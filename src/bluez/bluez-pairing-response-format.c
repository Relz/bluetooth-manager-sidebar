#include "bluez/bluez-pairing-response-format.h"

#include "bluez/bluez-pairing-response-validation.h"

GVariant *
bluez_pairing_response_format_pin_code(const char *text, const char **out_error_message)
{
  g_autofree char *pin_code = NULL;

  if (!bluez_pairing_response_validate_pin_code(text, &pin_code, out_error_message))
    return NULL;

  return g_variant_new("(s)", pin_code);
}

GVariant *
bluez_pairing_response_format_passkey(const char *text, const char **out_error_message)
{
  guint32 passkey = 0;

  if (!bluez_pairing_response_validate_passkey(text, &passkey, out_error_message))
    return NULL;

  return g_variant_new("(u)", passkey);
}
