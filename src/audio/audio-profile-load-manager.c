#include "audio/audio-profiles.h"

#include "audio/audio-profile-loader.h"
#include "audio/audio-profiles-internal.h"

static void
audio_profiles_load_finished(const char *device_path,
                             const char *address,
                             AudioProfileInfo *info,
                             gpointer user_data)
{
  AudioProfiles *profiles = user_data;

  if (!info)
    info = audio_profile_info_new_error("pactl-failed", NULL);

  info->loading = FALSE;
  g_free(info->address);
  info->address = g_strdup(address ? address : "");
  audio_profile_repository_replace(audio_profiles_repository(profiles), device_path, info);
  audio_profiles_notify_changed(profiles);
}

void
audio_profiles_load(AudioProfiles *profiles, const char *device_path, const char *address)
{
  if (audio_profiles_is_disposed(profiles) || !device_path || !*device_path)
    return;

  audio_profile_loader_start(audio_profiles_repository(profiles),
                             device_path,
                             address,
                             audio_profiles_load_finished,
                             audio_profiles_ref(profiles),
                             (GDestroyNotify)audio_profiles_unref);
}
