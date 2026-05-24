#pragma once

#include <glib.h>

typedef struct _BluezBatteryCache BluezBatteryCache;

BluezBatteryCache *bluez_battery_cache_new(void);
void bluez_battery_cache_free(BluezBatteryCache *cache);
void bluez_battery_cache_clear(BluezBatteryCache *cache);
void bluez_battery_cache_set(BluezBatteryCache *cache, const char *path, gint percentage);
void bluez_battery_cache_remove(BluezBatteryCache *cache, const char *path);
int bluez_battery_cache_get(BluezBatteryCache *cache, const char *path, gboolean *found);
