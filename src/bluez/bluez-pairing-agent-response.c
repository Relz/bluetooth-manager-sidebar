#include "bluez/bluez-pairing-agent-private.h"

#include "bluez/bluez-pairing-agent-session.h"
#include "bluez/bluez-pairing-response-dbus.h"
#include "bluez/bluez-pairing-response-format.h"

PairingResponse *
bluez_pairing_response_new(BluezPairingAgent *agent, GDBusMethodInvocation *invocation)
{
  PairingResponse *response = g_new0(PairingResponse, 1);

  response->ref_count = 1;
  response->agent = bluez_pairing_agent_ref(agent);
  response->invocation = g_object_ref(invocation);

  return response;
}

gpointer
bluez_pairing_response_ref(gpointer user_data)
{
  PairingResponse *response = user_data;

  response->ref_count++;
  return response;
}

void
bluez_pairing_response_unref(gpointer user_data)
{
  PairingResponse *response = user_data;

  if (!response)
    return;

  response->ref_count--;
  if (response->ref_count != 0)
    return;

  g_clear_object(&response->invocation);
  bluez_pairing_agent_unref(response->agent);
  g_free(response);
}

static void
bluez_pairing_agent_return_response(PairingResponse *response, GVariant *value)
{
  BluezPairingAgent *agent = response->agent;
  PairingPromptHandle *prompt = NULL;

  if (agent->disposed || !bluez_pairing_agent_session_take_active(agent, response, &prompt)) {
    bluez_pairing_response_dbus_discard_variant(value);
    return;
  }

  pairing_prompt_handle_free(prompt);
  bluez_pairing_response_dbus_return_value(response, value);
  bluez_pairing_response_unref(response);
}

static void
bluez_pairing_agent_reject_response(PairingResponse *response, const char *message)
{
  BluezPairingAgent *agent = response->agent;
  PairingPromptHandle *prompt = NULL;

  if (agent->disposed || !bluez_pairing_agent_session_take_active(agent, response, &prompt))
    return;

  pairing_prompt_handle_free(prompt);
  bluez_pairing_response_dbus_return_rejected(response, message);
  bluez_pairing_response_unref(response);
}

void
bluez_pairing_agent_close_active(BluezPairingAgent *agent,
                                 gboolean reject,
                                 const char *message)
{
  bluez_pairing_agent_session_clear_active(agent, reject, message);
}

void
bluez_pairing_response_accept(gpointer user_data)
{
  bluez_pairing_agent_return_response(user_data, NULL);
}

void
bluez_pairing_response_reject(gpointer user_data)
{
  bluez_pairing_agent_reject_response(user_data, PAIRING_REJECTED_MESSAGE);
}

void
bluez_pairing_response_accept_pin_code(const char *text, gpointer user_data)
{
  const char *error_message = NULL;
  GVariant *value = bluez_pairing_response_format_pin_code(text, &error_message);

  if (!value) {
    bluez_pairing_agent_reject_response(user_data, error_message);
    return;
  }

  bluez_pairing_agent_return_response(user_data, value);
}

void
bluez_pairing_response_accept_passkey(const char *text, gpointer user_data)
{
  const char *error_message = NULL;
  GVariant *value = bluez_pairing_response_format_passkey(text, &error_message);

  if (!value) {
    bluez_pairing_agent_reject_response(user_data, error_message);
    return;
  }

  bluez_pairing_agent_return_response(user_data, value);
}

void
bluez_pairing_agent_request_confirmation_prompt(BluezPairingAgent *agent,
                                                const char *heading,
                                                const char *body,
                                                GDBusMethodInvocation *invocation)
{
  PairingResponse *response = NULL;
  PairingPromptHandle *prompt = NULL;
  gpointer prompt_response = NULL;

  response = bluez_pairing_response_new(agent, invocation);
  bluez_pairing_agent_session_replace_active(agent, response, PAIRING_REPLACED_MESSAGE);
  prompt_response = bluez_pairing_response_ref(response);

  if (agent->callbacks.request_confirmation) {
    prompt = agent->callbacks.request_confirmation(heading,
                                                   body,
                                                   bluez_pairing_response_accept,
                                                   bluez_pairing_response_reject,
                                                   prompt_response,
                                                   bluez_pairing_response_unref,
                                                   agent->callbacks.user_data);
  }
  if (!prompt)
    bluez_pairing_response_unref(prompt_response);

  bluez_pairing_agent_session_track_prompt(agent, response, prompt);
  bluez_pairing_response_unref(response);
}

void
bluez_pairing_agent_request_entry_prompt(BluezPairingAgent *agent,
                                         const char *heading,
                                         const char *body,
                                         GDBusMethodInvocation *invocation,
                                         PairingEntryAccept accept,
                                         BluezPairingEntryPrompt request)
{
  PairingResponse *response = NULL;
  PairingPromptHandle *prompt = NULL;
  gpointer prompt_response = NULL;

  response = bluez_pairing_response_new(agent, invocation);
  bluez_pairing_agent_session_replace_active(agent, response, PAIRING_REPLACED_MESSAGE);
  prompt_response = bluez_pairing_response_ref(response);

  if (request) {
    prompt = request(heading,
                     body,
                     accept,
                     bluez_pairing_response_reject,
                     prompt_response,
                     bluez_pairing_response_unref,
                     agent->callbacks.user_data);
  }
  if (!prompt)
    bluez_pairing_response_unref(prompt_response);

  bluez_pairing_agent_session_track_prompt(agent, response, prompt);
  bluez_pairing_response_unref(response);
}
