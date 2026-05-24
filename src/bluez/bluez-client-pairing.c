#include "bluez/bluez-client-pairing.h"

#include "bluez/bluez-client-private.h"

const char *
bluez_client_pairing_agent_message(BluezClient *client)
{
  return bluez_client_pairing_service_message(bluez_client_services_pairing(client ? client->services : NULL));
}
