#pragma once

#include <adwaita.h>

#include "bluez/bluez-pairing-agent.h"

PairingPromptHandle *pairing_prompt_present(GtkWindow *window,
                                                    const char *heading,
                                                    const char *body,
                                                   GtkWidget *extra_child,
                                                   PairingAccept accept,
                                                   PairingEntryAccept entry_accept,
                                                    PairingReject reject,
                                                    gpointer response_data,
                                                    GDestroyNotify response_destroy);
PairingPromptHandle *pairing_prompt_present_entry(GtkWindow *window,
                                                     const char *heading,
                                                     const char *body,
                                                     gboolean passkey,
                                                     PairingEntryAccept accept,
                                                     PairingReject reject,
                                                     gpointer response_data,
                                                     GDestroyNotify response_destroy);
