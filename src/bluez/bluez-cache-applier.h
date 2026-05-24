#pragma once

#include "bluez/bluez-cache.h"

gboolean bluez_cache_applier_cache_interfaces(BluezObjectCache *cache,
                                              const char *path,
                                              GVariant *interfaces);
gboolean bluez_cache_applier_remove_interfaces(BluezObjectCache *cache,
                                               const char *path,
                                               GVariant *interfaces);
void bluez_cache_applier_apply_properties_changed(BluezObjectCache *cache,
                                                  const char *object_path,
                                                  const char *changed_interface,
                                                  GVariant *changed,
                                                  GVariant *invalidated);
