#include "bluez/bluez-client-private.h"

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

struct _BluezClientServices {
  BluezClient *client;
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezClientAdapterService *adapter;
  BluezClientDeviceService *devices;
  BluezClientPairingService *pairing;
  BluezClientQueryService *query;
};

static void
bluez_client_services_state_changed(gpointer user_data)
{
  BluezClientServices *services = user_data;

  bluez_client_notify_state_changed(services->client);
}

static void
bluez_client_services_pairing_changed(gpointer user_data)
{
  BluezClientServices *services = user_data;

  bluez_client_notify_state_changed(services->client);
}

static void
bluez_client_services_status(const char *message, gpointer user_data)
{
  BluezClientServices *services = user_data;

  bluez_client_notify_status(services->client, message);
}

static void
bluez_client_services_operation_result(const BluezOperationResult *result, gpointer user_data)
{
  BluezClientServices *services = user_data;

  bluez_client_notify_operation_result(services->client, result);
}

BluezClientServices *
bluez_client_services_new(BluezClient *client,
                          const BluezPairingAgentCallbacks *pairing_callbacks)
{
  BluezClientServices *services = g_new0(BluezClientServices, 1);

  services->client = client;
  services->state = bluez_state_new(bluez_client_services_state_changed, services, NULL);
  services->query = bluez_client_query_service_new(services->state);
  services->pairing = bluez_client_pairing_service_new(services->state,
                                                       pairing_callbacks,
                                                       bluez_client_services_status,
                                                       bluez_client_services_pairing_changed,
                                                       services);
  services->reporter = bluez_operation_reporter_new(bluez_client_services_status,
                                                    bluez_client_services_operation_result,
                                                     services,
                                                     NULL);
  services->adapter = bluez_client_adapter_service_new(services->state, services->reporter);
  services->devices = bluez_client_device_service_new(
    services->state,
    services->reporter,
    bluez_client_adapter_service_controller(services->adapter));

  return services;
}

void
bluez_client_services_free(BluezClientServices *services)
{
  if (!services)
    return;

  bluez_client_services_shutdown(services);
  g_clear_pointer(&services->devices, bluez_client_device_service_free);
  g_clear_pointer(&services->adapter, bluez_client_adapter_service_free);
  g_clear_pointer(&services->reporter, bluez_operation_reporter_free);
  g_clear_pointer(&services->pairing, bluez_client_pairing_service_free);
  g_clear_pointer(&services->query, bluez_client_query_service_free);
  g_clear_pointer(&services->state, bluez_state_free);
  g_free(services);
}

gboolean
bluez_client_services_connect(BluezClientServices *services)
{
  if (!services || !bluez_state_connect(services->state))
    return FALSE;

  bluez_client_pairing_service_start(services->pairing);
  return TRUE;
}

void
bluez_client_services_shutdown(BluezClientServices *services)
{
  if (!services)
    return;

  bluez_client_pairing_service_stop(services->pairing);
  bluez_client_adapter_service_shutdown(services->adapter);
  bluez_client_device_service_shutdown(services->devices);
  if (services->state)
    bluez_state_shutdown(services->state);
}

gboolean
bluez_client_services_refresh_state(BluezClientServices *services)
{
  return services ? bluez_state_refresh(services->state) : FALSE;
}

BluezClientAdapterService *
bluez_client_services_adapter(BluezClientServices *services)
{
  return services ? services->adapter : NULL;
}

BluezClientDeviceService *
bluez_client_services_devices(BluezClientServices *services)
{
  return services ? services->devices : NULL;
}

BluezClientPairingService *
bluez_client_services_pairing(BluezClientServices *services)
{
  return services ? services->pairing : NULL;
}

BluezClientQueryService *
bluez_client_services_query(BluezClientServices *services)
{
  return services ? services->query : NULL;
}
