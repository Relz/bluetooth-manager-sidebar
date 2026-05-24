#include "bluez/bluez-pairing-agent-private.h"

#include "bluez/bluez-pairing-prompts.h"

void
bluez_pairing_agent_display_pin_code(BluezPairingAgent *agent,
                                     const char *device_path,
                                     const char *pin_code)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *message = pairing_prompt_display_pin_code_message(device, pin_code);

  bluez_pairing_agent_show_info(agent, message);
}

void
bluez_pairing_agent_display_passkey(BluezPairingAgent *agent,
                                    const char *device_path,
                                    guint32 passkey,
                                    guint16 entered)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *message = pairing_prompt_display_passkey_message(device, passkey, entered);

  bluez_pairing_agent_show_info(agent, message);
}

void
bluez_pairing_agent_request_confirmation(BluezPairingAgent *agent,
                                         const char *device_path,
                                         guint32 passkey,
                                         GDBusMethodInvocation *invocation)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *heading = pairing_prompt_pair_heading(device);
  g_autofree char *body = pairing_prompt_pair_body(passkey);

  bluez_pairing_agent_request_confirmation_prompt(agent, heading, body, invocation);
}

void
bluez_pairing_agent_request_authorization(BluezPairingAgent *agent,
                                          const char *device_path,
                                          GDBusMethodInvocation *invocation)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *heading = pairing_prompt_authorization_heading(device);

  bluez_pairing_agent_request_confirmation_prompt(agent,
                                                 heading,
                                                 pairing_prompt_authorization_body(),
                                                 invocation);
}

void
bluez_pairing_agent_authorize_service(BluezPairingAgent *agent,
                                      const char *device_path,
                                      const char *uuid,
                                      GDBusMethodInvocation *invocation)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *heading = pairing_prompt_service_heading(device);
  g_autofree char *body = pairing_prompt_service_body(uuid);

  bluez_pairing_agent_request_confirmation_prompt(agent, heading, body, invocation);
}

void
bluez_pairing_agent_request_pin_code(BluezPairingAgent *agent,
                                     const char *device_path,
                                     GDBusMethodInvocation *invocation)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *heading = pairing_prompt_pin_heading(device);

  bluez_pairing_agent_request_entry_prompt(agent,
                                          heading,
                                          pairing_prompt_pin_body(),
                                          invocation,
                                          bluez_pairing_response_accept_pin_code,
                                          agent->callbacks.request_pin_code);
}

void
bluez_pairing_agent_request_passkey(BluezPairingAgent *agent,
                                    const char *device_path,
                                    GDBusMethodInvocation *invocation)
{
  g_autofree char *device = bluez_pairing_agent_device_name(agent, device_path);
  g_autofree char *heading = pairing_prompt_passkey_heading(device);

  bluez_pairing_agent_request_entry_prompt(agent,
                                          heading,
                                          pairing_prompt_passkey_body(),
                                          invocation,
                                          bluez_pairing_response_accept_passkey,
                                          agent->callbacks.request_passkey);
}
