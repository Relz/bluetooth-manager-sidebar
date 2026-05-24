#pragma once

#include <gio/gio.h>

GVariant *bluez_pairing_response_format_pin_code(const char *text,
                                                 const char **out_error_message);
GVariant *bluez_pairing_response_format_passkey(const char *text,
                                                const char **out_error_message);
