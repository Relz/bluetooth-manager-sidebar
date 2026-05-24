#include "bluez/bluez-properties.h"

struct _BluezProperties {
  GHashTable *values;
};

static GVariant *
bluez_variant_ref_unboxed(GVariant *value)
{
  if (!value)
    return NULL;

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_VARIANT))
    return g_variant_get_variant(value);

  return g_variant_ref_sink(value);
}

static void
bluez_properties_set_owned_variant(BluezProperties *props,
                                       const char *name,
                                       GVariant *value)
{
  if (!props || !name || !*name || !value) {
    if (value)
      g_variant_unref(value);
    return;
  }

  g_hash_table_replace(props->values, g_strdup(name), value);
}

BluezProperties *
bluez_properties_new(void)
{
  BluezProperties *props = g_new0(BluezProperties, 1);

  props->values = g_hash_table_new_full(g_str_hash,
                                        g_str_equal,
                                        g_free,
                                        (GDestroyNotify)g_variant_unref);

  return props;
}

BluezProperties *
bluez_properties_new_from_variant_dict(GVariant *dict)
{
  BluezProperties *props = bluez_properties_new();

  if (!dict || !g_variant_is_of_type(dict, G_VARIANT_TYPE_VARDICT))
    return props;

  for (gsize index = 0; index < g_variant_n_children(dict); index++) {
    g_autoptr(GVariant) entry = g_variant_get_child_value(dict, index);
    g_autoptr(GVariant) key = g_variant_get_child_value(entry, 0);
    g_autoptr(GVariant) value = g_variant_get_child_value(entry, 1);
    const char *name = g_variant_get_string(key, NULL);

    bluez_properties_set_owned_variant(props, name, bluez_variant_ref_unboxed(value));
  }

  return props;
}

BluezProperties *
bluez_properties_copy(BluezProperties *props)
{
  if (!props)
    return NULL;

  BluezProperties *copy = bluez_properties_new();
  GHashTableIter iter;
  gpointer key = NULL;
  gpointer value = NULL;

  g_hash_table_iter_init(&iter, props->values);
  while (g_hash_table_iter_next(&iter, &key, &value))
    bluez_properties_set_variant(copy, key, value);

  return copy;
}

void
bluez_properties_free(BluezProperties *props)
{
  if (!props)
    return;

  g_hash_table_destroy(props->values);
  g_free(props);
}

void
bluez_properties_set_variant(BluezProperties *props,
                                 const char *name,
                                 GVariant *value)
{
  if (!props || !name || !*name)
    return;

  if (!value) {
    bluez_properties_remove(props, name);
    return;
  }

  g_hash_table_replace(props->values, g_strdup(name), g_variant_ref_sink(value));
}

void
bluez_properties_remove(BluezProperties *props, const char *name)
{
  if (!props || !name)
    return;

  g_hash_table_remove(props->values, name);
}

gboolean
bluez_properties_get_bool(BluezProperties *props,
                              const char *name,
                              gboolean fallback)
{
  GVariant *value = bluez_properties_lookup(props, name);

  if (value && g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN))
    return g_variant_get_boolean(value);

  return fallback;
}

gboolean
bluez_properties_lookup_bool(BluezProperties *props,
                                 const char *name,
                                 gboolean *out_value)
{
  GVariant *value = bluez_properties_lookup(props, name);

  if (!value || !g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN))
    return FALSE;

  if (out_value)
    *out_value = g_variant_get_boolean(value);

  return TRUE;
}

char *
bluez_properties_dup_string(BluezProperties *props, const char *name)
{
  GVariant *value = bluez_properties_lookup(props, name);

  if (!value)
    return NULL;

  if (!g_variant_is_of_type(value, G_VARIANT_TYPE_STRING) &&
      !g_variant_is_of_type(value, G_VARIANT_TYPE_OBJECT_PATH) &&
      !g_variant_is_of_type(value, G_VARIANT_TYPE_SIGNATURE))
    return NULL;

  return g_strdup(g_variant_get_string(value, NULL));
}

char **
bluez_properties_dup_strv(BluezProperties *props, const char *name)
{
  GVariant *value = bluez_properties_lookup(props, name);
  char **result;

  if (!value || !g_variant_is_of_type(value, G_VARIANT_TYPE("as")))
    return NULL;

  result = g_new0(char *, g_variant_n_children(value) + 1);
  for (gsize index = 0; index < g_variant_n_children(value); index++) {
    g_autoptr(GVariant) child = g_variant_get_child_value(value, index);

    result[index] = g_strdup(g_variant_get_string(child, NULL));
  }

  return result;
}

static gboolean
bluez_variant_get_int(GVariant *value, gint *out_value)
{
  gint64 signed_value = 0;
  guint64 unsigned_value = 0;
  gboolean is_unsigned = FALSE;

  if (!value)
    return FALSE;

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_BYTE)) {
    unsigned_value = g_variant_get_byte(value);
    is_unsigned = TRUE;
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT16)) {
    signed_value = g_variant_get_int16(value);
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT16)) {
    unsigned_value = g_variant_get_uint16(value);
    is_unsigned = TRUE;
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT32)) {
    signed_value = g_variant_get_int32(value);
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT32)) {
    unsigned_value = g_variant_get_uint32(value);
    is_unsigned = TRUE;
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT64)) {
    signed_value = g_variant_get_int64(value);
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT64)) {
    unsigned_value = g_variant_get_uint64(value);
    is_unsigned = TRUE;
  } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_HANDLE)) {
    signed_value = g_variant_get_handle(value);
  } else {
    return FALSE;
  }

  if (is_unsigned) {
    *out_value = unsigned_value > G_MAXINT ? G_MAXINT : (gint)unsigned_value;
    return TRUE;
  }

  if (signed_value > G_MAXINT)
    *out_value = G_MAXINT;
  else if (signed_value < G_MININT)
    *out_value = G_MININT;
  else
    *out_value = (gint)signed_value;

  return TRUE;
}

gint
bluez_properties_get_int(BluezProperties *props,
                             const char *name,
                             gboolean *found)
{
  gint value = 0;
  gboolean has_value = bluez_variant_get_int(bluez_properties_lookup(props, name), &value);

  if (found)
    *found = has_value;

  return has_value ? value : 0;
}

GVariant *
bluez_properties_lookup(BluezProperties *props, const char *name)
{
  if (!props || !name)
    return NULL;

  return g_hash_table_lookup(props->values, name);
}
