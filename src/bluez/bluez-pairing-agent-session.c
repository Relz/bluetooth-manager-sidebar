#include "bluez/bluez-pairing-agent-session.h"

#include "bluez/bluez-pairing-response-dbus.h"

static void
bluez_pairing_agent_session_reject_response(gpointer user_data, const char *message)
{
  bluez_pairing_response_dbus_return_rejected(user_data, message);
}

void
bluez_pairing_agent_session_replace_active(BluezPairingAgent *agent,
                                           PairingResponse *response,
                                           const char *message)
{
  pairing_prompt_session_replace_response(&agent->prompt_session,
                                          response,
                                          bluez_pairing_response_ref,
                                          bluez_pairing_response_unref,
                                          message,
                                          bluez_pairing_agent_session_reject_response);
}

gboolean
bluez_pairing_agent_session_take_active(BluezPairingAgent *agent,
                                        PairingResponse *response,
                                        PairingPromptHandle **prompt)
{
  return agent && pairing_prompt_session_take_response(&agent->prompt_session, response, prompt);
}

void
bluez_pairing_agent_session_clear_active(BluezPairingAgent *agent,
                                         gboolean reject,
                                         const char *message)
{
  if (!agent)
    return;

  pairing_prompt_session_clear(&agent->prompt_session,
                               reject,
                               message,
                               bluez_pairing_agent_session_reject_response);
}

void
bluez_pairing_agent_session_track_prompt(BluezPairingAgent *agent,
                                         PairingResponse *response,
                                         PairingPromptHandle *prompt)
{
  if (!prompt) {
    if (pairing_prompt_session_has_response(&agent->prompt_session, response)) {
      pairing_prompt_session_take_response(&agent->prompt_session, response, NULL);
      bluez_pairing_response_dbus_return_rejected(response, PAIRING_UNAVAILABLE_MESSAGE);
      bluez_pairing_response_unref(response);
    }
    return;
  }

  if (pairing_prompt_session_has_response(&agent->prompt_session, response)) {
    pairing_prompt_session_set_prompt(&agent->prompt_session, prompt);
    return;
  }

  pairing_prompt_handle_close(prompt);
  pairing_prompt_handle_free(prompt);
}
