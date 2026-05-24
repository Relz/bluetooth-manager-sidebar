#pragma once

#include "pairing/pairing-prompt-session.h"

typedef struct _PairingPromptResponse PairingPromptResponse;

PairingPromptResponse *pairing_prompt_response_new(PairingAccept accept,
                                                      PairingEntryAccept entry_accept,
                                                      PairingReject reject,
                                                      gpointer response_data,
                                                      GDestroyNotify response_destroy);
PairingPromptResponse *pairing_prompt_response_ref(PairingPromptResponse *response);
void pairing_prompt_response_unref(PairingPromptResponse *response);
void pairing_prompt_response_accept(PairingPromptResponse *response, const char *text);
void pairing_prompt_response_reject(PairingPromptResponse *response);
