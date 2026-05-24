#include "audio/audio-profiles.h"

#include "audio/audio-profiles-internal.h"

AudioProfileInfo *
audio_profiles_dup_info(AudioProfiles *profiles, const char *device_path)
{
  AudioProfileInfo *info = NULL;
  AudioProfileRepository *repository = audio_profiles_repository(profiles);

  if (repository && device_path)
    info = audio_profile_repository_dup_info(repository, device_path);
  if (info)
    return info;

  info = audio_profile_info_new();
  info->error = g_strdup("Audio profile information has not been loaded.");
  return info;
}
