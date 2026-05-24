#include "bluez/bluez-pairing-agent-manager.h"

#include "bluez/bluez-pairing-agent-contract.h"

#define BLUEZ_BUS_NAME "org.bluez"
#define BLUEZ_ROOT_PATH "/org/bluez"
#define BLUEZ_AGENT_MANAGER_INTERFACE "org.bluez.AgentManager1"
#define BLUEZ_AGENT_CALL_TIMEOUT_MS 5000
#define BLUEZ_AGENT_UNREGISTER_TIMEOUT_MS 1000

void
bluez_pairing_agent_manager_register_agent(GDBusConnection *connection,
                                           GAsyncReadyCallback callback,
                                           gpointer user_data)
{
  g_dbus_connection_call(connection,
                         BLUEZ_BUS_NAME,
                         BLUEZ_ROOT_PATH,
                         BLUEZ_AGENT_MANAGER_INTERFACE,
                         "RegisterAgent",
                         g_variant_new("(os)",
                                       bluez_pairing_agent_contract_object_path(),
                                       bluez_pairing_agent_contract_capability()),
                         NULL,
                         G_DBUS_CALL_FLAGS_NONE,
                         BLUEZ_AGENT_CALL_TIMEOUT_MS,
                         NULL,
                         callback,
                         user_data);
}

gboolean
bluez_pairing_agent_manager_register_agent_finish(GDBusConnection *connection,
                                                  GAsyncResult *result,
                                                  GError **error)
{
  g_autoptr(GVariant) reply = NULL;

  reply = g_dbus_connection_call_finish(connection, result, error);
  return reply != NULL;
}

void
bluez_pairing_agent_manager_request_default_agent(GDBusConnection *connection,
                                                  GAsyncReadyCallback callback,
                                                  gpointer user_data)
{
  g_dbus_connection_call(connection,
                         BLUEZ_BUS_NAME,
                         BLUEZ_ROOT_PATH,
                         BLUEZ_AGENT_MANAGER_INTERFACE,
                         "RequestDefaultAgent",
                         g_variant_new("(o)", bluez_pairing_agent_contract_object_path()),
                         NULL,
                         G_DBUS_CALL_FLAGS_NONE,
                         BLUEZ_AGENT_CALL_TIMEOUT_MS,
                         NULL,
                         callback,
                         user_data);
}

gboolean
bluez_pairing_agent_manager_request_default_agent_finish(GDBusConnection *connection,
                                                         GAsyncResult *result,
                                                         GError **error)
{
  g_autoptr(GVariant) reply = NULL;

  reply = g_dbus_connection_call_finish(connection, result, error);
  return reply != NULL;
}

gboolean
bluez_pairing_agent_manager_unregister_agent(GDBusConnection *connection,
                                             GError **error)
{
  g_autoptr(GVariant) reply = NULL;

  if (!connection)
    return FALSE;

  reply = g_dbus_connection_call_sync(connection,
                                      BLUEZ_BUS_NAME,
                                      BLUEZ_ROOT_PATH,
                                      BLUEZ_AGENT_MANAGER_INTERFACE,
                                      "UnregisterAgent",
                                      g_variant_new("(o)", bluez_pairing_agent_contract_object_path()),
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      BLUEZ_AGENT_UNREGISTER_TIMEOUT_MS,
                                      NULL,
                                      error);

  return reply != NULL;
}
