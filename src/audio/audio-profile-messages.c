#include "audio/audio-profile-messages.h"

char *
audio_profile_load_error_message(const char *reason, const char *detail)
{
  if (g_strcmp0(reason, "missing-address") == 0)
    return g_strdup("Device address unavailable.");
  if (g_strcmp0(reason, "missing-card") == 0)
    return g_strdup("No audio card is available for this device.");
  if (g_strcmp0(reason, "missing-profiles") == 0)
    return g_strdup("No audio profiles are available for this device.");
  if (g_strcmp0(reason, "pactl-unavailable") == 0)
    return g_strdup("pactl is unavailable.");
  if (g_strcmp0(reason, "pactl-timeout") == 0)
    return g_strdup("Unable to load audio profiles: pactl timed out.");
  if (g_strcmp0(reason, "pactl-invalid-json") == 0)
    return g_strdup("Unable to load audio profiles: pactl returned invalid JSON.");
  if (g_strcmp0(reason, "pactl-unexpected-output") == 0)
    return g_strdup("Unable to load audio profiles: unexpected pactl output.");
  if (detail && *detail)
    return g_strdup_printf("Unable to load audio profiles: %s", detail);
  return g_strdup("Unable to load audio profiles.");
}

char *
audio_profile_change_message(const char *reason, const char *detail)
{
  if (!reason)
    return g_strdup("Audio profile changed");
  if (g_strcmp0(reason, "pactl-unavailable") == 0)
    return g_strdup("Unable to change audio profile: pactl is unavailable");
  if (g_strcmp0(reason, "pactl-timeout") == 0)
    return g_strdup("Unable to change audio profile: pactl timed out");
  if (detail && *detail)
    return g_strdup_printf("Unable to change audio profile: %s", detail);
  return g_strdup("Unable to change audio profile");
}
