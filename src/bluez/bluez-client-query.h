#pragma once

#include "bluez/bluez-cache.h"
#include "bluez/bluez-client.h"

BluezObjectCache *bluez_client_cache(BluezClient *client);
const char *bluez_client_primary_adapter(BluezClient *client);
char **bluez_client_dup_adapter_device_paths(BluezClient *client,
                                             const char *adapter_path);
const char *bluez_client_error(BluezClient *client);
int bluez_client_device_battery(BluezClient *client,
                                const char *device_path,
                                gboolean *found);
