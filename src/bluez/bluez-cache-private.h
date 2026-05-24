#pragma once

#include "bluez/bluez-cache.h"

gboolean bluez_object_cache_path_is_valid(const char *path);
gboolean bluez_object_cache_contains_adapter(BluezObjectCache *cache, const char *path);
char **bluez_object_cache_dup_adapter_paths_sorted(BluezObjectCache *cache);
char **bluez_object_cache_dup_device_paths_sorted(BluezObjectCache *cache);

void bluez_object_cache_replace_adapter(BluezObjectCache *cache,
                                        const char *path,
                                        BluezProperties *properties);
void bluez_object_cache_replace_device(BluezObjectCache *cache,
                                       const char *path,
                                       BluezProperties *properties);
gboolean bluez_object_cache_remove_adapter(BluezObjectCache *cache, const char *path);
void bluez_object_cache_remove_device(BluezObjectCache *cache, const char *path);

void bluez_object_cache_set_device_battery(BluezObjectCache *cache,
                                           const char *path,
                                           int percentage);
void bluez_object_cache_remove_device_battery(BluezObjectCache *cache, const char *path);
int bluez_object_cache_device_battery_value(BluezObjectCache *cache,
                                            const char *path,
                                            gboolean *found);

void bluez_object_cache_set_primary_adapter_path(BluezObjectCache *cache, const char *path);
