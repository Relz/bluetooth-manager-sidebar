#include "bluez/bluez-client-query-service.h"

#include "bluez/bluez-cache-query.h"

struct _BluezClientQueryService {
  BluezState *state;
};

BluezClientQueryService *
bluez_client_query_service_new(BluezState *state)
{
  BluezClientQueryService *service = g_new0(BluezClientQueryService, 1);

  service->state = state;
  return service;
}

void
bluez_client_query_service_free(BluezClientQueryService *service)
{
  g_free(service);
}

BluezObjectCache *
bluez_client_query_service_cache(BluezClientQueryService *service)
{
  return service ? bluez_state_cache(service->state) : NULL;
}

const char *
bluez_client_query_service_primary_adapter(BluezClientQueryService *service)
{
  return service ? bluez_state_primary_adapter(service->state) : NULL;
}

char **
bluez_client_query_service_dup_adapter_device_paths(BluezClientQueryService *service,
                                                    const char *adapter_path)
{
  return bluez_cache_query_dup_device_paths(bluez_client_query_service_cache(service), adapter_path);
}

const char *
bluez_client_query_service_error(BluezClientQueryService *service)
{
  return service ? bluez_state_error(service->state) : NULL;
}

int
bluez_client_query_service_device_battery(BluezClientQueryService *service,
                                          const char *device_path,
                                          gboolean *found)
{
  if (!service) {
    if (found)
      *found = FALSE;
    return 0;
  }

  return bluez_cache_query_device_battery(bluez_client_query_service_cache(service),
                                          device_path,
                                          found);
}
