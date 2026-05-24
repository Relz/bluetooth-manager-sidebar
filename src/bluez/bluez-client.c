#include "bluez/bluez-client-private.h"

void
bluez_client_notify_state_changed(BluezClient *client)
{
  if (client && client->on_state_changed)
    client->on_state_changed(client->user_data);
}

void
bluez_client_notify_status(BluezClient *client, const char *message)
{
  if (client && client->on_status)
    client->on_status(message, client->user_data);
}

void
bluez_client_notify_operation_result(BluezClient *client,
                                     const BluezOperationResult *result)
{
  if (client && client->on_operation_result)
    client->on_operation_result(result, client->user_data);
}

BluezClient *
bluez_client_new(BluezClientStateChanged on_state_changed,
                 BluezClientStatus on_status,
                 BluezClientOperationResult on_operation_result,
                 const BluezPairingAgentCallbacks *pairing_callbacks,
                 gpointer user_data,
                 GDestroyNotify destroy)
{
  BluezClient *client = g_new0(BluezClient, 1);

  client->on_state_changed = on_state_changed;
  client->on_status = on_status;
  client->on_operation_result = on_operation_result;
  client->user_data = user_data;
  client->destroy = destroy;
  client->services = bluez_client_services_new(client, pairing_callbacks);

  return client;
}

void
bluez_client_free(BluezClient *client)
{
  if (!client)
    return;

  g_clear_pointer(&client->services, bluez_client_services_free);
  if (client->destroy && client->user_data)
    client->destroy(client->user_data);
  g_free(client);
}

gboolean
bluez_client_connect(BluezClient *client)
{
  return client ? bluez_client_services_connect(client->services) : FALSE;
}

void
bluez_client_shutdown(BluezClient *client)
{
  if (client)
    bluez_client_services_shutdown(client->services);
}

gboolean
bluez_client_refresh_state(BluezClient *client)
{
  return client ? bluez_client_services_refresh_state(client->services) : FALSE;
}
