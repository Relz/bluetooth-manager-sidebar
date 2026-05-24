#include "bluez/bluez-client-device.h"

#include "bluez/bluez-client-private.h"

gboolean
bluez_client_is_device_connecting(BluezClient *client, const char *device_path)
{
  return client &&
         bluez_client_device_service_is_connecting(bluez_client_services_devices(client->services),
                                                   device_path);
}

void
bluez_client_activate_device(BluezClient *client, const char *device_path)
{
  if (client)
    bluez_client_device_service_activate(bluez_client_services_devices(client->services), device_path);
}

void
bluez_client_trust_device(BluezClient *client, const char *device_path)
{
  if (client)
    bluez_client_device_service_trust(bluez_client_services_devices(client->services), device_path);
}

void
bluez_client_set_device_alias(BluezClient *client, const char *device_path, const char *alias)
{
  if (client)
    bluez_client_device_service_set_alias(bluez_client_services_devices(client->services), device_path, alias);
}

void
bluez_client_set_device_trusted(BluezClient *client, const char *device_path, gboolean trusted)
{
  if (client)
    bluez_client_device_service_set_trusted(bluez_client_services_devices(client->services), device_path, trusted);
}

void
bluez_client_set_device_blocked(BluezClient *client, const char *device_path, gboolean blocked)
{
  if (client)
    bluez_client_device_service_set_blocked(bluez_client_services_devices(client->services), device_path, blocked);
}

void
bluez_client_forget_device(BluezClient *client, const char *device_path)
{
  if (client)
    bluez_client_device_service_forget(bluez_client_services_devices(client->services), device_path);
}
