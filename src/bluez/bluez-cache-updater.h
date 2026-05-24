#pragma once

#include "bluez/bluez-cache.h"

void bluez_cache_updater_replace_from_managed_objects(BluezObjectCache *cache,
                                                      GVariant *managed_objects);
void bluez_cache_updater_cache_interfaces(BluezObjectCache *cache,
                                          const char *path,
                                          GVariant *interfaces);
void bluez_cache_updater_remove_interfaces(BluezObjectCache *cache,
                                           const char *path,
                                           GVariant *interfaces);
void bluez_cache_updater_apply_properties_changed(BluezObjectCache *cache,
                                                  const char *object_path,
                                                  const char *changed_interface,
                                                  GVariant *changed,
                                                  GVariant *invalidated);
