#include "bluez/bluez-client-query.h"

#include "bluez/bluez-client-private.h"

BluezObjectCache *
bluez_client_cache(BluezClient *client)
{
  return bluez_client_query_service_cache(bluez_client_services_query(client ? client->services : NULL));
}

const char *
bluez_client_primary_adapter(BluezClient *client)
{
  return bluez_client_query_service_primary_adapter(bluez_client_services_query(client ? client->services : NULL));
}

char **
bluez_client_dup_adapter_device_paths(BluezClient *client, const char *adapter_path)
{
  return bluez_client_query_service_dup_adapter_device_paths(
    bluez_client_services_query(client ? client->services : NULL),
    adapter_path);
}

const char *
bluez_client_error(BluezClient *client)
{
  return bluez_client_query_service_error(bluez_client_services_query(client ? client->services : NULL));
}

int
bluez_client_device_battery(BluezClient *client,
                            const char *device_path,
                            gboolean *found)
{
  return bluez_client_query_service_device_battery(
    bluez_client_services_query(client ? client->services : NULL),
    device_path,
    found);
}
