#include "bluez/bluez-pairing-agent-private.h"

#include <string.h>

BluezPairingAgent *
bluez_pairing_agent_ref(BluezPairingAgent *agent)
{
  agent->ref_count++;
  return agent;
}

void
bluez_pairing_agent_unref(BluezPairingAgent *agent)
{
  agent->ref_count--;
  if (agent->ref_count == 0)
    g_free(agent);
}

char *
bluez_pairing_agent_device_name(BluezPairingAgent *agent, const char *device_path)
{
  char *name = NULL;
  const char *fallback = NULL;

  if (agent->callbacks.device_name) {
    name = agent->callbacks.device_name(device_path, agent->callbacks.user_data);
    if (name && *name)
      return name;
    g_free(name);
  }

  if (device_path)
    fallback = strrchr(device_path, '/');
  if (fallback && fallback[1])
    return g_strdup(fallback + 1);

  return g_strdup("Bluetooth device");
}

void
bluez_pairing_agent_show_info(BluezPairingAgent *agent, const char *message)
{
  if (agent && !agent->disposed && agent->callbacks.show_info)
    agent->callbacks.show_info(message, agent->callbacks.user_data);
}

static void
bluez_pairing_agent_registration_status(const char *message, gpointer user_data)
{
  BluezPairingAgent *agent = user_data;

  if (!agent || agent->disposed || !agent->callbacks.set_agent_message)
    return;

  agent->callbacks.set_agent_message(message, agent->callbacks.user_data);
}

BluezPairingAgent *
bluez_pairing_agent_new(GDBusConnection *connection,
                            const BluezPairingAgentCallbacks *callbacks)
{
  BluezPairingAgent *agent = NULL;

  g_return_val_if_fail(G_IS_DBUS_CONNECTION(connection), NULL);

  agent = g_new0(BluezPairingAgent, 1);
  agent->ref_count = 1;
  pairing_prompt_session_init(&agent->prompt_session);
  if (callbacks)
    agent->callbacks = *callbacks;
  agent->registration = bluez_pairing_agent_registration_new(connection,
                                                             bluez_pairing_agent_dbus_vtable(),
                                                             bluez_pairing_agent_ref(agent),
                                                             (GDestroyNotify)bluez_pairing_agent_unref,
                                                            bluez_pairing_agent_registration_status,
                                                            agent);

  return agent;
}

void
bluez_pairing_agent_free(BluezPairingAgent *agent)
{
  if (!agent)
    return;

  if (!agent->disposed) {
    agent->disposed = TRUE;
    bluez_pairing_agent_unregister(agent);
    g_clear_pointer(&agent->registration, bluez_pairing_agent_registration_free);
    agent->callbacks = (BluezPairingAgentCallbacks){ 0 };
  }

  bluez_pairing_agent_unref(agent);
}

gboolean
bluez_pairing_agent_register(BluezPairingAgent *agent)
{
  g_return_val_if_fail(agent != NULL, FALSE);

  return !agent->disposed &&
    bluez_pairing_agent_registration_register(agent->registration);
}

void
bluez_pairing_agent_unregister(BluezPairingAgent *agent)
{
  if (!agent)
    return;

  bluez_pairing_agent_close_active(agent, TRUE, PAIRING_CANCELLED_MESSAGE);
  bluez_pairing_agent_registration_unregister(agent->registration);
}
