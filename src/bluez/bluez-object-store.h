#pragma once

#include "bluez/bluez-properties.h"

typedef struct _BluezObjectStore BluezObjectStore;

BluezObjectStore *bluez_object_store_new(void);
void bluez_object_store_free(BluezObjectStore *store);
void bluez_object_store_clear(BluezObjectStore *store);

gboolean bluez_object_store_path_is_valid(const char *path);
gboolean bluez_object_store_contains_adapter(BluezObjectStore *store, const char *path);
char **bluez_object_store_dup_adapter_paths_sorted(BluezObjectStore *store);
char **bluez_object_store_dup_device_paths_sorted(BluezObjectStore *store);

void bluez_object_store_replace_adapter(BluezObjectStore *store,
                                        const char *path,
                                        BluezProperties *properties);
void bluez_object_store_replace_device(BluezObjectStore *store,
                                       const char *path,
                                       BluezProperties *properties);
gboolean bluez_object_store_remove_adapter(BluezObjectStore *store, const char *path);
void bluez_object_store_remove_device(BluezObjectStore *store, const char *path);

void bluez_object_store_set_device_battery(BluezObjectStore *store,
                                           const char *path,
                                           int percentage);
void bluez_object_store_remove_device_battery(BluezObjectStore *store, const char *path);
int bluez_object_store_device_battery_value(BluezObjectStore *store,
                                            const char *path,
                                            gboolean *found);

BluezProperties *bluez_object_store_adapter(BluezObjectStore *store,
                                               const char *path);
BluezProperties *bluez_object_store_device(BluezObjectStore *store,
                                              const char *path);
