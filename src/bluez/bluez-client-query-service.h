#pragma once

#include "bluez/bluez-cache.h"
#include "bluez/bluez-state.h"

typedef struct _BluezClientQueryService BluezClientQueryService;

BluezClientQueryService *bluez_client_query_service_new(BluezState *state);
void bluez_client_query_service_free(BluezClientQueryService *service);

BluezObjectCache *bluez_client_query_service_cache(BluezClientQueryService *service);
const char *bluez_client_query_service_primary_adapter(BluezClientQueryService *service);
char **bluez_client_query_service_dup_adapter_device_paths(BluezClientQueryService *service,
                                                           const char *adapter_path);
const char *bluez_client_query_service_error(BluezClientQueryService *service);
int bluez_client_query_service_device_battery(BluezClientQueryService *service,
                                              const char *device_path,
                                              gboolean *found);
