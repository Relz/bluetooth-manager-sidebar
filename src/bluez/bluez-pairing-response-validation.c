#include "bluez/bluez-pairing-response-validation.h"

#include "bluez/bluez-pairing-prompts.h"

gboolean
bluez_pairing_response_validate_pin_code(const char *text,
                                         char **out_pin_code,
                                         const char **out_error_message)
{
  if (pairing_prompt_validate_pin_code(text, out_pin_code))
    return TRUE;

  if (out_error_message)
    *out_error_message = "Invalid PIN code";
  return FALSE;
}

gboolean
bluez_pairing_response_validate_passkey(const char *text,
                                        guint32 *out_passkey,
                                        const char **out_error_message)
{
  if (pairing_prompt_validate_passkey(text, out_passkey))
    return TRUE;

  if (out_error_message)
    *out_error_message = "Invalid passkey";
  return FALSE;
}
