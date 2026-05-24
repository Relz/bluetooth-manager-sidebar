#pragma once

#include "bluez/bluez-cache.h"

char **bluez_cache_query_dup_adapter_paths(BluezObjectCache *cache);
char **bluez_cache_query_dup_device_paths(BluezObjectCache *cache,
                                          const char *adapter_path);
int bluez_cache_query_device_battery(BluezObjectCache *cache,
                                     const char *path,
                                     gboolean *found);
char *bluez_cache_query_dup_device_adapter_path(BluezObjectCache *cache,
                                                const char *device_path);
