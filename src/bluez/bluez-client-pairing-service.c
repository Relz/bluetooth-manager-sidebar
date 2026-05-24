#include "bluez/bluez-client-pairing-service.h"

struct _BluezClientPairingService {
  BluezState *state;
  BluezPairingClientAdapter *adapter;
  BluezPairingAgent *agent;
};

BluezClientPairingService *
bluez_client_pairing_service_new(BluezState *state,
                                 const BluezPairingAgentCallbacks *pairing_callbacks,
                                 BluezPairingClientAdapterStatus status,
                                 BluezPairingClientAdapterChanged changed,
                                 gpointer user_data)
{
  BluezClientPairingService *service = g_new0(BluezClientPairingService, 1);

  service->state = state;
  service->adapter = bluez_pairing_client_adapter_new(state,
                                                      pairing_callbacks,
                                                      status,
                                                      changed,
                                                      user_data);
  return service;
}

void
bluez_client_pairing_service_free(BluezClientPairingService *service)
{
  if (!service)
    return;

  bluez_client_pairing_service_stop(service);
  g_clear_pointer(&service->adapter, bluez_pairing_client_adapter_free);
  g_free(service);
}

void
bluez_client_pairing_service_start(BluezClientPairingService *service)
{
  GDBusConnection *connection = NULL;
  BluezPairingAgentCallbacks callbacks = { 0 };

  if (!service || service->agent)
    return;

  connection = bluez_state_connection(service->state);
  if (!connection)
    return;

  bluez_pairing_client_adapter_init_agent_callbacks(service->adapter, &callbacks);
  service->agent = bluez_pairing_agent_new(connection, &callbacks);
  bluez_pairing_agent_register(service->agent);
}

void
bluez_client_pairing_service_stop(BluezClientPairingService *service)
{
  if (service)
    g_clear_pointer(&service->agent, bluez_pairing_agent_free);
}

const char *
bluez_client_pairing_service_message(BluezClientPairingService *service)
{
  return service ? bluez_pairing_client_adapter_message(service->adapter) : NULL;
}
