#include "bluez/bluez-battery-cache.h"

struct _BluezBatteryCache {
  GHashTable *percentages;
};

BluezBatteryCache *
bluez_battery_cache_new(void)
{
  BluezBatteryCache *cache = g_new0(BluezBatteryCache, 1);

  cache->percentages = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  return cache;
}

void
bluez_battery_cache_free(BluezBatteryCache *cache)
{
  if (!cache)
    return;

  g_hash_table_destroy(cache->percentages);
  g_free(cache);
}

void
bluez_battery_cache_clear(BluezBatteryCache *cache)
{
  if (cache)
    g_hash_table_remove_all(cache->percentages);
}

void
bluez_battery_cache_set(BluezBatteryCache *cache, const char *path, gint percentage)
{
  if (cache && path)
    g_hash_table_replace(cache->percentages, g_strdup(path), GINT_TO_POINTER(percentage));
}

void
bluez_battery_cache_remove(BluezBatteryCache *cache, const char *path)
{
  if (cache && path)
    g_hash_table_remove(cache->percentages, path);
}

int
bluez_battery_cache_get(BluezBatteryCache *cache, const char *path, gboolean *found)
{
  gpointer value = NULL;
  gboolean has_value = cache && path &&
    g_hash_table_lookup_extended(cache->percentages, path, NULL, &value);

  if (found)
    *found = has_value;

  return has_value ? GPOINTER_TO_INT(value) : 0;
}
