#pragma once

#include "app/application.h"

#include "bluez/bluez-pairing-agent.h"

void application_pairing_presenter_show_info(Application *self, const char *message);
PairingPromptHandle *application_pairing_presenter_request_confirmation(Application *self,
                                                                           const char *heading,
                                                                           const char *body,
                                                                           PairingAccept accept,
                                                                           PairingReject reject,
                                                                           gpointer response_data,
                                                                           GDestroyNotify response_destroy);
PairingPromptHandle *application_pairing_presenter_request_pin_code(Application *self,
                                                                       const char *heading,
                                                                       const char *body,
                                                                       PairingEntryAccept accept,
                                                                       PairingReject reject,
                                                                       gpointer response_data,
                                                                       GDestroyNotify response_destroy);
PairingPromptHandle *application_pairing_presenter_request_passkey(Application *self,
                                                                      const char *heading,
                                                                      const char *body,
                                                                      PairingEntryAccept accept,
                                                                      PairingReject reject,
                                                                      gpointer response_data,
                                                                      GDestroyNotify response_destroy);
