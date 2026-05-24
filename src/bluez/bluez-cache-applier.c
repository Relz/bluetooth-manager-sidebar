#include "bluez/bluez-cache-applier.h"

#include "bluez/bluez-cache-private.h"
#include "bluez/bluez-dbus-variants.h"

#define BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter1"
#define BLUEZ_DEVICE_INTERFACE "org.bluez.Device1"
#define BLUEZ_BATTERY_INTERFACE "org.bluez.Battery1"

static void
bluez_cache_applier_set_battery_percentage(BluezObjectCache *cache,
                                           const char *path,
                                           GVariant *properties)
{
  BluezProperties *props = bluez_properties_new_from_variant_dict(properties);
  gboolean found = FALSE;
  gint percentage = bluez_properties_get_int(props, "Percentage", &found);

  if (found)
    bluez_object_cache_set_device_battery(cache, path, percentage);

  bluez_properties_free(props);
}

static void
bluez_cache_applier_apply_property_changes(BluezProperties *props,
                                           GVariant *changed,
                                           GVariant *invalidated)
{
  if (!props)
    return;

  if (invalidated && g_variant_is_of_type(invalidated, G_VARIANT_TYPE("as"))) {
    for (gsize index = 0; index < g_variant_n_children(invalidated); index++) {
      g_autoptr(GVariant) child = g_variant_get_child_value(invalidated, index);
      bluez_properties_remove(props, g_variant_get_string(child, NULL));
    }
  }

  if (!changed || !g_variant_is_of_type(changed, G_VARIANT_TYPE_VARDICT))
    return;

  for (gsize index = 0; index < g_variant_n_children(changed); index++) {
    g_autoptr(GVariant) entry = g_variant_get_child_value(changed, index);
    g_autoptr(GVariant) key = g_variant_get_child_value(entry, 0);
    g_autoptr(GVariant) value = g_variant_get_child_value(entry, 1);
    GVariant *unboxed = bluez_dbus_variant_ref_unboxed(value);

    bluez_properties_set_variant(props, g_variant_get_string(key, NULL), unboxed);
    if (unboxed)
      g_variant_unref(unboxed);
  }
}

gboolean
bluez_cache_applier_cache_interfaces(BluezObjectCache *cache,
                                     const char *path,
                                     GVariant *interfaces)
{
  if (!cache || !bluez_object_cache_path_is_valid(path) ||
      !interfaces || !g_variant_is_of_type(interfaces, G_VARIANT_TYPE("a{sa{sv}}")))
    return FALSE;

  g_autoptr(GVariant) adapter = bluez_dbus_dict_lookup_child_value(interfaces, BLUEZ_ADAPTER_INTERFACE);
  g_autoptr(GVariant) device = bluez_dbus_dict_lookup_child_value(interfaces, BLUEZ_DEVICE_INTERFACE);
  g_autoptr(GVariant) battery = bluez_dbus_dict_lookup_child_value(interfaces, BLUEZ_BATTERY_INTERFACE);

  if (adapter) {
    bluez_object_cache_replace_adapter(cache,
                                       path,
                                       bluez_properties_new_from_variant_dict(adapter));
  }

  if (device) {
    bluez_object_cache_replace_device(cache,
                                      path,
                                      bluez_properties_new_from_variant_dict(device));
  }

  if (battery)
    bluez_cache_applier_set_battery_percentage(cache, path, battery);

  return adapter != NULL;
}

gboolean
bluez_cache_applier_remove_interfaces(BluezObjectCache *cache,
                                      const char *path,
                                      GVariant *interfaces)
{
  gboolean removed_adapter = FALSE;

  if (!cache || !bluez_object_cache_path_is_valid(path) ||
      !interfaces || !g_variant_is_of_type(interfaces, G_VARIANT_TYPE("as")))
    return FALSE;

  for (gsize index = 0; index < g_variant_n_children(interfaces); index++) {
    g_autoptr(GVariant) child = g_variant_get_child_value(interfaces, index);
    const char *interface = g_variant_get_string(child, NULL);

    if (g_str_equal(interface, BLUEZ_ADAPTER_INTERFACE)) {
      removed_adapter = bluez_object_cache_remove_adapter(cache, path) || removed_adapter;
    } else if (g_str_equal(interface, BLUEZ_DEVICE_INTERFACE)) {
      bluez_object_cache_remove_device(cache, path);
    } else if (g_str_equal(interface, BLUEZ_BATTERY_INTERFACE)) {
      bluez_object_cache_remove_device_battery(cache, path);
    }
  }

  return removed_adapter;
}

void
bluez_cache_applier_apply_properties_changed(BluezObjectCache *cache,
                                             const char *object_path,
                                             const char *changed_interface,
                                             GVariant *changed,
                                             GVariant *invalidated)
{
  if (!cache || !bluez_object_cache_path_is_valid(object_path) || !changed_interface)
    return;

  if (g_str_equal(changed_interface, BLUEZ_ADAPTER_INTERFACE)) {
    bluez_cache_applier_apply_property_changes(bluez_object_cache_adapter(cache, object_path),
                                               changed,
                                               invalidated);
  } else if (g_str_equal(changed_interface, BLUEZ_DEVICE_INTERFACE)) {
    bluez_cache_applier_apply_property_changes(bluez_object_cache_device(cache, object_path),
                                               changed,
                                               invalidated);
  } else if (g_str_equal(changed_interface, BLUEZ_BATTERY_INTERFACE)) {
    gint percentage = 0;

    if (bluez_dbus_string_array_contains(invalidated, "Percentage"))
      bluez_object_cache_remove_device_battery(cache, object_path);

    if (bluez_dbus_changed_dict_get_int(changed, "Percentage", &percentage))
      bluez_object_cache_set_device_battery(cache, object_path, percentage);
  }
}
