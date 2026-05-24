#pragma once

#include <glib.h>

#include "bluez/bluez-properties.h"

typedef struct _BluezObjectCache BluezObjectCache;

BluezObjectCache *bluez_object_cache_new(void);
void bluez_object_cache_free(BluezObjectCache *cache);
void bluez_object_cache_clear(BluezObjectCache *cache);

void bluez_object_cache_replace_from_managed_objects(BluezObjectCache *cache,
                                                         GVariant *managed_objects);
void bluez_object_cache_cache_interfaces(BluezObjectCache *cache,
                                             const char *path,
                                             GVariant *interfaces);
void bluez_object_cache_remove_interfaces(BluezObjectCache *cache,
                                              const char *path,
                                              GVariant *interfaces);
void bluez_object_cache_apply_properties_changed(BluezObjectCache *cache,
                                                     const char *object_path,
                                                     const char *changed_interface,
                                                     GVariant *changed,
                                                     GVariant *invalidated);

const char *bluez_object_cache_primary_adapter(BluezObjectCache *cache);
BluezProperties *bluez_object_cache_adapter(BluezObjectCache *cache,
                                                   const char *path);
BluezProperties *bluez_object_cache_device(BluezObjectCache *cache,
                                                  const char *path);

void bluez_object_cache_set_error(BluezObjectCache *cache, const char *message);
const char *bluez_object_cache_error(BluezObjectCache *cache);
