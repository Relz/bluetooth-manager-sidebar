#pragma once

#include <gio/gio.h>

typedef struct _BluezPairingAgentObject BluezPairingAgentObject;

BluezPairingAgentObject *bluez_pairing_agent_object_new(
  GDBusConnection *connection,
  const GDBusInterfaceVTable *vtable,
  gpointer user_data,
  GDestroyNotify destroy);
void bluez_pairing_agent_object_free(BluezPairingAgentObject *object);

GDBusConnection *bluez_pairing_agent_object_connection(BluezPairingAgentObject *object);
gboolean bluez_pairing_agent_object_is_exported(BluezPairingAgentObject *object);
gboolean bluez_pairing_agent_object_export(BluezPairingAgentObject *object, GError **error);
void bluez_pairing_agent_object_unexport(BluezPairingAgentObject *object);
