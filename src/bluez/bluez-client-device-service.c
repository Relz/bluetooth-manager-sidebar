#include "bluez/bluez-client-device-service.h"

#include "bluez/bluez-device-activation.h"
#include "bluez/bluez-device-connection.h"
#include "bluez/bluez-device-pair-trust-connect.h"
#include "bluez/bluez-device-properties.h"
#include "bluez/bluez-device-removal.h"

struct _BluezClientDeviceService {
  BluezState *state;
  BluezDeviceConnectionController *connections;
  BluezDevicePairTrustConnectWorkflow *pair_trust_connect;
  BluezDevicePropertiesController *properties;
  BluezDeviceRemovalController *removal;
};

static void bluez_client_device_service_connect(BluezClientDeviceService *service,
                                                const char *device_path);
static void bluez_client_device_service_disconnect(BluezClientDeviceService *service,
                                                   const char *device_path);
static void bluez_client_device_service_pair_trust_and_connect(BluezClientDeviceService *service,
                                                               const char *device_path);

static void
bluez_client_device_connect_callback(const char *device_path, gpointer user_data)
{
  bluez_client_device_service_connect(user_data, device_path);
}

static void
bluez_client_device_disconnect_callback(const char *device_path, gpointer user_data)
{
  bluez_client_device_service_disconnect(user_data, device_path);
}

static void
bluez_client_device_pair_trust_connect_callback(const char *device_path, gpointer user_data)
{
  bluez_client_device_service_pair_trust_and_connect(user_data, device_path);
}

BluezClientDeviceService *
bluez_client_device_service_new(BluezState *state,
                                BluezOperationReporter *reporter,
                                BluezAdapterController *adapter)
{
  BluezClientDeviceService *service = g_new0(BluezClientDeviceService, 1);

  service->state = state;
  service->connections = bluez_device_connection_controller_new(state, reporter, adapter);
  service->properties = bluez_device_properties_controller_new(state, reporter);
  service->pair_trust_connect = bluez_device_pair_trust_connect_workflow_new(
    state,
    reporter,
    bluez_device_connection_controller_power_gate(service->connections),
    service->properties,
    bluez_client_device_connect_callback,
    service,
    NULL);
  service->removal = bluez_device_removal_controller_new(state, reporter);
  return service;
}

void
bluez_client_device_service_free(BluezClientDeviceService *service)
{
  if (!service)
    return;

  bluez_client_device_service_shutdown(service);
  g_clear_pointer(&service->removal, bluez_device_removal_controller_free);
  g_clear_pointer(&service->pair_trust_connect, bluez_device_pair_trust_connect_workflow_free);
  g_clear_pointer(&service->properties, bluez_device_properties_controller_free);
  g_clear_pointer(&service->connections, bluez_device_connection_controller_free);
  g_free(service);
}

void
bluez_client_device_service_shutdown(BluezClientDeviceService *service)
{
  if (service && service->connections)
    bluez_device_connection_controller_shutdown(service->connections);
}

gboolean
bluez_client_device_service_is_connecting(BluezClientDeviceService *service, const char *device_path)
{
  return service && bluez_device_connection_controller_is_connecting(service->connections, device_path);
}

void
bluez_client_device_service_activate(BluezClientDeviceService *service, const char *device_path)
{
  BluezDeviceActivationActions actions = {
    .connect = bluez_client_device_connect_callback,
    .disconnect = bluez_client_device_disconnect_callback,
    .pair_trust_and_connect = bluez_client_device_pair_trust_connect_callback,
    .user_data = service,
  };

  if (service)
    bluez_device_activation_activate(service->state, device_path, &actions);
}

static void
bluez_client_device_service_connect(BluezClientDeviceService *service, const char *device_path)
{
  if (service)
    bluez_device_connection_controller_connect(service->connections, device_path);
}

static void
bluez_client_device_service_disconnect(BluezClientDeviceService *service, const char *device_path)
{
  if (service)
    bluez_device_connection_controller_disconnect(service->connections, device_path);
}

static void
bluez_client_device_service_pair_trust_and_connect(BluezClientDeviceService *service,
                                                   const char *device_path)
{
  if (service)
    bluez_device_pair_trust_connect_workflow_start(service->pair_trust_connect, device_path);
}

void
bluez_client_device_service_trust(BluezClientDeviceService *service, const char *device_path)
{
  if (service)
    bluez_device_properties_controller_trust(service->properties, device_path);
}

void
bluez_client_device_service_set_alias(BluezClientDeviceService *service,
                                      const char *device_path,
                                      const char *alias)
{
  if (service)
    bluez_device_properties_controller_set_alias(service->properties, device_path, alias);
}

void
bluez_client_device_service_set_trusted(BluezClientDeviceService *service,
                                        const char *device_path,
                                        gboolean trusted)
{
  if (service)
    bluez_device_properties_controller_set_trusted(service->properties, device_path, trusted);
}

void
bluez_client_device_service_set_blocked(BluezClientDeviceService *service,
                                        const char *device_path,
                                        gboolean blocked)
{
  if (service)
    bluez_device_properties_controller_set_blocked(service->properties, device_path, blocked);
}

void
bluez_client_device_service_forget(BluezClientDeviceService *service, const char *device_path)
{
  if (service)
    bluez_device_removal_controller_forget(service->removal, device_path);
}
