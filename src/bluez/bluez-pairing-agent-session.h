#pragma once

#include "bluez/bluez-pairing-agent-private.h"

void bluez_pairing_agent_session_replace_active(BluezPairingAgent *agent,
                                                PairingResponse *response,
                                                const char *message);
gboolean bluez_pairing_agent_session_take_active(BluezPairingAgent *agent,
                                                 PairingResponse *response,
                                                 PairingPromptHandle **prompt);
void bluez_pairing_agent_session_clear_active(BluezPairingAgent *agent,
                                              gboolean reject,
                                              const char *message);
void bluez_pairing_agent_session_track_prompt(BluezPairingAgent *agent,
                                              PairingResponse *response,
                                              PairingPromptHandle *prompt);
