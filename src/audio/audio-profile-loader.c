#include "audio/audio-profile-loader.h"

#include "audio/audio-profiles-internal.h"

gboolean
audio_profile_loader_start(AudioProfileRepository *repository,
                           const char *device_path,
                           const char *address,
                           AudioProfileLoadTaskFinished callback,
                           gpointer user_data,
                           GDestroyNotify destroy)
{
  AudioProfileInfo *cached;
  AudioProfileInfo *loading;

  if (!repository || !device_path || !*device_path) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  cached = audio_profile_repository_lookup(repository, device_path);
  if (cached && cached->loading) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }
  if (cached && !cached->error &&
      g_strcmp0(cached->address, address ? address : "") == 0) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  if (!address || !*address) {
    AudioProfileInfo *info = audio_profile_info_new_error("missing-address", NULL);

    info->address = g_strdup("");
    audio_profile_repository_replace(repository, device_path, info);
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  loading = audio_profile_info_new();
  loading->loading = TRUE;
  loading->address = g_strdup(address);
  audio_profile_repository_replace(repository, device_path, loading);

  audio_profile_load_task_start(device_path,
                                address,
                                callback,
                                user_data,
                                destroy);
  return TRUE;
}
