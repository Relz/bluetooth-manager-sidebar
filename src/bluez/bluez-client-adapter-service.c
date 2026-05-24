#include "bluez/bluez-client-adapter-service.h"

#include "bluez/bluez-adapter-power-workflow.h"
#include "bluez/bluez-discovery.h"

struct _BluezClientAdapterService {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezAdapterController *adapter;
  BluezDiscoveryController *discovery;
};

BluezClientAdapterService *
bluez_client_adapter_service_new(BluezState *state, BluezOperationReporter *reporter)
{
  BluezClientAdapterService *service = g_new0(BluezClientAdapterService, 1);

  service->state = state;
  service->reporter = reporter;
  service->adapter = bluez_adapter_controller_new(state, reporter);
  service->discovery = bluez_discovery_controller_new(state, reporter, service->adapter);
  return service;
}

void
bluez_client_adapter_service_free(BluezClientAdapterService *service)
{
  if (!service)
    return;

  bluez_client_adapter_service_shutdown(service);
  g_clear_pointer(&service->discovery, bluez_discovery_controller_free);
  g_clear_pointer(&service->adapter, bluez_adapter_controller_free);
  g_free(service);
}

void
bluez_client_adapter_service_shutdown(BluezClientAdapterService *service)
{
  if (service && service->discovery)
    bluez_discovery_controller_shutdown(service->discovery);
}

BluezAdapterController *
bluez_client_adapter_service_controller(BluezClientAdapterService *service)
{
  return service ? service->adapter : NULL;
}

void
bluez_client_adapter_service_set_power(BluezClientAdapterService *service, gboolean powered)
{
  if (service)
    bluez_adapter_power_workflow_set_power(service->state,
                                           service->reporter,
                                           service->adapter,
                                           powered);
}

void
bluez_client_adapter_service_set_discoverable(BluezClientAdapterService *service,
                                              gboolean discoverable)
{
  if (service)
    bluez_adapter_controller_set_discoverable(service->adapter, discoverable);
}

void
bluez_client_adapter_service_toggle_discovery(BluezClientAdapterService *service)
{
  if (service)
    bluez_discovery_controller_toggle(service->discovery);
}
