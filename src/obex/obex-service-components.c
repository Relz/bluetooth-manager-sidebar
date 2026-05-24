#include "obex/obex-service-components.h"

struct _ObexServiceComponents {
  ObexAgentLifecycle *agent;
  ObexAuthorizationStore *authorizations;
  ObexTransferRegistry *transfers;
  ObexTransferMonitor *transfer_monitor;
  ObexServiceContext *context;
};

ObexServiceComponents *
obex_service_components_new(const ObexServiceDependencies *dependencies,
                            gpointer user_data,
                            GDestroyNotify destroy)
{
  ObexServiceComponents *components = g_new0(ObexServiceComponents, 1);

  components->agent = obex_agent_lifecycle_new();
  components->authorizations = obex_authorization_store_new();
  components->transfers = obex_transfer_registry_new();
  components->context = obex_service_context_new(dependencies, user_data, destroy);
  return components;
}

void
obex_service_components_free(ObexServiceComponents *components)
{
  if (!components)
    return;

  g_clear_pointer(&components->transfer_monitor, obex_transfer_monitor_free);
  g_clear_pointer(&components->transfers, obex_transfer_registry_free);
  g_clear_pointer(&components->authorizations, obex_authorization_store_free);
  g_clear_pointer(&components->agent, obex_agent_lifecycle_free);
  g_clear_pointer(&components->context, obex_service_context_free);
  g_free(components);
}

ObexAgentLifecycle *
obex_service_components_agent(ObexServiceComponents *components)
{
  return components ? components->agent : NULL;
}

ObexAuthorizationStore *
obex_service_components_authorizations(ObexServiceComponents *components)
{
  return components ? components->authorizations : NULL;
}

ObexTransferRegistry *
obex_service_components_transfers(ObexServiceComponents *components)
{
  return components ? components->transfers : NULL;
}

ObexServiceContext *
obex_service_components_context(ObexServiceComponents *components)
{
  return components ? components->context : NULL;
}

void
obex_service_components_take_transfer_monitor(ObexServiceComponents *components,
                                              ObexTransferMonitor *monitor)
{
  if (!components) {
    obex_transfer_monitor_free(monitor);
    return;
  }

  g_clear_pointer(&components->transfer_monitor, obex_transfer_monitor_free);
  components->transfer_monitor = monitor;
}

void
obex_service_components_clear_transfer_monitor(ObexServiceComponents *components)
{
  if (components)
    g_clear_pointer(&components->transfer_monitor, obex_transfer_monitor_free);
}
