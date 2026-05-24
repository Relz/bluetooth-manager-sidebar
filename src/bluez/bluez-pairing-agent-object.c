#include "bluez/bluez-pairing-agent-object.h"

#include "bluez/bluez-pairing-agent-contract.h"

struct _BluezPairingAgentObject {
  GDBusConnection *connection;
  GDBusNodeInfo *node_info;
  const GDBusInterfaceVTable *vtable;
  gpointer user_data;
  GDestroyNotify destroy;
  guint registration_id;
};

BluezPairingAgentObject *
bluez_pairing_agent_object_new(GDBusConnection *connection,
                               const GDBusInterfaceVTable *vtable,
                               gpointer user_data,
                               GDestroyNotify destroy)
{
  BluezPairingAgentObject *object = NULL;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail(G_IS_DBUS_CONNECTION(connection), NULL);
  g_return_val_if_fail(vtable != NULL, NULL);

  object = g_new0(BluezPairingAgentObject, 1);
  object->connection = g_object_ref(connection);
  object->vtable = vtable;
  object->user_data = user_data;
  object->destroy = destroy;
  object->node_info = bluez_pairing_agent_contract_node_info_new(&error);
  if (!object->node_info)
    g_error("Invalid BlueZ pairing agent introspection XML: %s", error->message);

  return object;
}

void
bluez_pairing_agent_object_free(BluezPairingAgentObject *object)
{
  if (!object)
    return;

  bluez_pairing_agent_object_unexport(object);
  if (object->destroy && object->user_data)
    object->destroy(object->user_data);
  g_clear_pointer(&object->node_info, g_dbus_node_info_unref);
  g_clear_object(&object->connection);
  g_free(object);
}

GDBusConnection *
bluez_pairing_agent_object_connection(BluezPairingAgentObject *object)
{
  return object ? object->connection : NULL;
}

gboolean
bluez_pairing_agent_object_is_exported(BluezPairingAgentObject *object)
{
  return object && object->registration_id != 0;
}

gboolean
bluez_pairing_agent_object_export(BluezPairingAgentObject *object, GError **error)
{
  if (!object || !object->connection || !object->node_info)
    return FALSE;

  if (object->registration_id != 0)
    return TRUE;

  object->registration_id = g_dbus_connection_register_object(
    object->connection,
    bluez_pairing_agent_contract_object_path(),
    object->node_info->interfaces[0],
    object->vtable,
    object->user_data,
    NULL,
    error);
  return object->registration_id != 0;
}

void
bluez_pairing_agent_object_unexport(BluezPairingAgentObject *object)
{
  if (!object || !object->registration_id || !object->connection)
    return;

  g_dbus_connection_unregister_object(object->connection, object->registration_id);
  object->registration_id = 0;
}
