#pragma once

#include <glib.h>

char *pairing_prompt_display_pin_code_message(const char *device, const char *pin_code);
char *pairing_prompt_display_passkey_message(const char *device,
                                                 guint32 passkey,
                                                 guint16 entered);
char *pairing_prompt_pair_heading(const char *device);
char *pairing_prompt_pair_body(guint32 passkey);
char *pairing_prompt_authorization_heading(const char *device);
const char *pairing_prompt_authorization_body(void);
char *pairing_prompt_service_heading(const char *device);
char *pairing_prompt_service_body(const char *uuid);
char *pairing_prompt_pin_heading(const char *device);
const char *pairing_prompt_pin_body(void);
char *pairing_prompt_passkey_heading(const char *device);
const char *pairing_prompt_passkey_body(void);

gboolean pairing_prompt_validate_pin_code(const char *text, char **out_pin_code);
gboolean pairing_prompt_validate_passkey(const char *text, guint32 *out_passkey);
