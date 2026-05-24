#pragma once

#include <gio/gio.h>

typedef struct _BluezPairingAgentRegistration BluezPairingAgentRegistration;

typedef void (*BluezPairingAgentRegistrationStatus)(const char *message, gpointer user_data);

BluezPairingAgentRegistration *bluez_pairing_agent_registration_new(
  GDBusConnection *connection,
  const GDBusInterfaceVTable *vtable,
  gpointer object_user_data,
  GDestroyNotify object_destroy,
  BluezPairingAgentRegistrationStatus status,
  gpointer status_user_data);
void bluez_pairing_agent_registration_free(BluezPairingAgentRegistration *registration);

gboolean bluez_pairing_agent_registration_register(BluezPairingAgentRegistration *registration);
void bluez_pairing_agent_registration_unregister(BluezPairingAgentRegistration *registration);
