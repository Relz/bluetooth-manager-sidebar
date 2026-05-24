#include "app/application-device-presentation.h"

#include "core/display.h"

static gboolean
string_has_text(const char *value)
{
  return value && *value;
}

static char *
application_join_strv(char **values)
{
  GString *joined;

  if (!values || !values[0])
    return g_strdup("None");

  joined = g_string_new(NULL);
  for (guint i = 0; values[i]; i++) {
    g_autofree char *display = display_dup_safe_text(values[i], NULL);

    if (!display)
      continue;
    if (joined->len > 0)
      g_string_append(joined, ", ");
    g_string_append(joined, display);
  }

  if (joined->len == 0) {
    g_string_free(joined, TRUE);
    return g_strdup("None");
  }

  return g_string_free(joined, FALSE);
}

char *
application_device_presentation_dup_property_display(BluezProperties *props, const char *name)
{
  GVariant *value = bluez_properties_lookup(props, name);

  if (!value)
    return g_strdup("Unknown");

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN))
    return g_strdup(g_variant_get_boolean(value) ? "Yes" : "No");

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING) ||
      g_variant_is_of_type(value, G_VARIANT_TYPE_OBJECT_PATH) ||
      g_variant_is_of_type(value, G_VARIANT_TYPE_SIGNATURE)) {
    const char *text = g_variant_get_string(value, NULL);

    return display_dup_safe_text(string_has_text(text) ? text : NULL, "Unknown");
  }

  if (g_variant_is_of_type(value, G_VARIANT_TYPE("as"))) {
    g_auto(GStrv) strv = bluez_properties_dup_strv(props, name);

    return application_join_strv(strv);
  }

  if (g_variant_is_of_type(value, G_VARIANT_TYPE_BYTE))
    return g_strdup_printf("%u", (guint)g_variant_get_byte(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT16))
    return g_strdup_printf("%d", (gint)g_variant_get_int16(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT16))
    return g_strdup_printf("%u", (guint)g_variant_get_uint16(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT32))
    return g_strdup_printf("%d", g_variant_get_int32(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT32))
    return g_strdup_printf("%u", g_variant_get_uint32(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT64))
    return g_strdup_printf("%" G_GINT64_FORMAT, g_variant_get_int64(value));
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT64))
    return g_strdup_printf("%" G_GUINT64_FORMAT, g_variant_get_uint64(value));

  {
    g_autofree char *printed = g_variant_print(value, FALSE);
    return display_dup_safe_text(printed, "Unknown");
  }
}
