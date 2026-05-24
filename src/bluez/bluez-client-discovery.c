#include "bluez/bluez-client-discovery.h"

#include "bluez/bluez-client-private.h"

void
bluez_client_toggle_discovery(BluezClient *client)
{
  if (client)
    bluez_client_adapter_service_toggle_discovery(bluez_client_services_adapter(client->services));
}
