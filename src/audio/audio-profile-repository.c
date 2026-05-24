#include "audio/audio-profile-repository.h"

#include "audio/audio-profile-cache.h"

struct _AudioProfileRepository {
  AudioProfileCache *cache;
};

AudioProfileRepository *
audio_profile_repository_new(void)
{
  AudioProfileRepository *repository = g_new0(AudioProfileRepository, 1);

  repository->cache = audio_profile_cache_new();
  return repository;
}

void
audio_profile_repository_free(AudioProfileRepository *repository)
{
  if (!repository)
    return;

  audio_profile_cache_free(repository->cache);
  g_free(repository);
}

AudioProfileInfo *
audio_profile_repository_lookup(AudioProfileRepository *repository, const char *device_path)
{
  return repository ? audio_profile_cache_lookup(repository->cache, device_path) : NULL;
}

AudioProfileInfo *
audio_profile_repository_dup_info(AudioProfileRepository *repository, const char *device_path)
{
  return repository ? audio_profile_cache_dup_info(repository->cache, device_path) : NULL;
}

void
audio_profile_repository_replace(AudioProfileRepository *repository,
                                 const char *device_path,
                                 AudioProfileInfo *info)
{
  if (!repository) {
    audio_profile_info_free(info);
    return;
  }

  audio_profile_cache_replace(repository->cache, device_path, info);
}

void
audio_profile_repository_update_active_profile(AudioProfileRepository *repository,
                                               const char *card_name,
                                               const char *profile_name)
{
  if (repository)
    audio_profile_cache_update_active_profile(repository->cache, card_name, profile_name);
}
