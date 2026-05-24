#include "bluez/bluez-device-display.h"

#include "bluez/bluez-cache-query.h"
#include "core/display.h"

char *
bluez_device_dup_display_name(BluezProperties *device, const char *fallback)
{
  static const char *display_keys[] = { "Alias", "Name", "Address" };

  if (device) {
    for (guint index = 0; index < G_N_ELEMENTS(display_keys); index++) {
      char *value = bluez_properties_dup_string(device, display_keys[index]);

      if (value && *value) {
        g_autofree char *safe = display_dup_safe_text(value, NULL);

        if (safe) {
          g_free(value);
          return g_steal_pointer(&safe);
        }
      }
      g_free(value);
    }
  }

  return display_dup_safe_text(fallback, NULL);
}

char *
bluez_object_cache_dup_device_display_name(BluezObjectCache *cache,
                                           const char *device_path,
                                           const char *fallback)
{
  return bluez_device_dup_display_name(bluez_object_cache_device(cache, device_path), fallback);
}

char *
bluez_object_cache_dup_device_display_name_for_address(BluezObjectCache *cache,
                                                       const char *address,
                                                       const char *fallback)
{
  if (!cache || !address || !*address)
    return fallback ? g_strdup(fallback) : NULL;

  g_autofree char *normalized = g_utf8_casefold(address, -1);
  g_auto(GStrv) paths = bluez_cache_query_dup_device_paths(cache, NULL);

  for (guint i = 0; paths && paths[i]; i++) {
    BluezProperties *device = bluez_object_cache_device(cache, paths[i]);
    g_autofree char *device_address = bluez_properties_dup_string(device, "Address");

    if (device_address) {
      g_autofree char *device_normalized = g_utf8_casefold(device_address, -1);

      if (g_str_equal(device_normalized, normalized))
        return bluez_device_dup_display_name(device, fallback);
    }
  }

  return fallback ? g_strdup(fallback) : NULL;
}
