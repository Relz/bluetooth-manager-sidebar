#include "bluez/bluez-dbus-variants.h"

#include "bluez/bluez-properties.h"

GVariant *
bluez_dbus_variant_ref_unboxed(GVariant *value)
{
  if (!value)
    return NULL;

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_VARIANT))
    return g_variant_get_variant(value);

  return g_variant_ref_sink(value);
}

GVariant *
bluez_dbus_dict_lookup_child_value(GVariant *dict, const char *name)
{
  if (!dict || !name)
    return NULL;

  for (gsize index = 0; index < g_variant_n_children(dict); index++) {
    g_autoptr(GVariant) entry = g_variant_get_child_value(dict, index);
    g_autoptr(GVariant) key = g_variant_get_child_value(entry, 0);
    const char *entry_name = g_variant_get_string(key, NULL);

    if (g_str_equal(entry_name, name))
      return g_variant_get_child_value(entry, 1);
  }

  return NULL;
}

gboolean
bluez_dbus_string_array_contains(GVariant *array, const char *needle)
{
  if (!array || !needle || !g_variant_is_of_type(array, G_VARIANT_TYPE("as")))
    return FALSE;

  for (gsize index = 0; index < g_variant_n_children(array); index++) {
    g_autoptr(GVariant) child = g_variant_get_child_value(array, index);
    const char *value = g_variant_get_string(child, NULL);

    if (g_str_equal(value, needle))
      return TRUE;
  }

  return FALSE;
}

gboolean
bluez_dbus_changed_dict_get_int(GVariant *changed, const char *name, gint *out_value)
{
  if (!changed || !g_variant_is_of_type(changed, G_VARIANT_TYPE_VARDICT))
    return FALSE;

  g_autoptr(GVariant) value = bluez_dbus_dict_lookup_child_value(changed, name);
  if (!value)
    return FALSE;

  g_autoptr(GVariant) unboxed = bluez_dbus_variant_ref_unboxed(value);
  BluezProperties *props = bluez_properties_new();
  gboolean found = FALSE;
  gint result = 0;

  bluez_properties_set_variant(props, name, unboxed);
  result = bluez_properties_get_int(props, name, &found);
  bluez_properties_free(props);

  if (found)
    *out_value = result;

  return found;
}
