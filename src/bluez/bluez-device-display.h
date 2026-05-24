#pragma once

#include "bluez/bluez-cache.h"
#include "bluez/bluez-properties.h"

char *bluez_device_dup_display_name(BluezProperties *device, const char *fallback);
char *bluez_object_cache_dup_device_display_name(BluezObjectCache *cache,
                                                      const char *device_path,
                                                      const char *fallback);
char *bluez_object_cache_dup_device_display_name_for_address(BluezObjectCache *cache,
                                                             const char *address,
                                                             const char *fallback);
