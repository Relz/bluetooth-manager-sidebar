#include "bluez/bluez-client-adapter.h"

#include "bluez/bluez-client-private.h"

void
bluez_client_set_adapter_power(BluezClient *client, gboolean powered)
{
  if (client)
    bluez_client_adapter_service_set_power(bluez_client_services_adapter(client->services), powered);
}

void
bluez_client_set_adapter_discoverable(BluezClient *client, gboolean discoverable)
{
  if (client)
    bluez_client_adapter_service_set_discoverable(bluez_client_services_adapter(client->services), discoverable);
}
