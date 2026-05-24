#include "bluez/bluez-pairing-prompts.h"

char *
pairing_prompt_display_pin_code_message(const char *device, const char *pin_code)
{
  return g_strdup_printf("Pairing code for %s: %s", device, pin_code);
}

char *
pairing_prompt_display_passkey_message(const char *device, guint32 passkey, guint16 entered)
{
  return g_strdup_printf("Pairing code for %s: %06u (%u entered)",
                         device,
                         passkey,
                         (guint)entered);
}

char *
pairing_prompt_pair_heading(const char *device)
{
  return g_strdup_printf("Pair with %s?", device);
}

char *
pairing_prompt_pair_body(guint32 passkey)
{
  return g_strdup_printf("Confirm that this code is shown on the device: %06u", passkey);
}

char *
pairing_prompt_authorization_heading(const char *device)
{
  return g_strdup_printf("Authorize %s?", device);
}

const char *
pairing_prompt_authorization_body(void)
{
  return "Allow this Bluetooth device to pair with this computer?";
}

char *
pairing_prompt_service_heading(const char *device)
{
  return g_strdup_printf("Authorize service for %s?", device);
}

char *
pairing_prompt_service_body(const char *uuid)
{
  return g_strdup_printf("Allow this device to use service %s?", uuid);
}

char *
pairing_prompt_pin_heading(const char *device)
{
  return g_strdup_printf("Enter PIN for %s", device);
}

const char *
pairing_prompt_pin_body(void)
{
  return "Enter the PIN code shown by the Bluetooth device.";
}

char *
pairing_prompt_passkey_heading(const char *device)
{
  return g_strdup_printf("Enter passkey for %s", device);
}

const char *
pairing_prompt_passkey_body(void)
{
  return "Enter the 6-digit passkey shown by the Bluetooth device.";
}

gboolean
pairing_prompt_validate_pin_code(const char *text, char **out_pin_code)
{
  g_autofree char *stripped = g_strdup(text ? text : "");

  g_strstrip(stripped);
  if (!*stripped)
    return FALSE;

  if (out_pin_code)
    *out_pin_code = g_steal_pointer(&stripped);
  return TRUE;
}

gboolean
pairing_prompt_validate_passkey(const char *text, guint32 *out_passkey)
{
  g_autofree char *stripped = g_strdup(text ? text : "");
  guint64 passkey = 0;

  g_strstrip(stripped);
  if (!g_ascii_string_to_unsigned(stripped, 10, 0, 999999, &passkey, NULL))
    return FALSE;

  if (out_passkey)
    *out_passkey = (guint32)passkey;
  return TRUE;
}
