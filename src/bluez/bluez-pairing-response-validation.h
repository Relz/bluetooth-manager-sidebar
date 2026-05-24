#pragma once

#include <gio/gio.h>

gboolean bluez_pairing_response_validate_pin_code(const char *text,
                                                  char **out_pin_code,
                                                  const char **out_error_message);
gboolean bluez_pairing_response_validate_passkey(const char *text,
                                                 guint32 *out_passkey,
                                                 const char **out_error_message);
