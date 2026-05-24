#include "audio/audio-profiles.h"

#include "audio/audio-profile-repository.h"
#include "audio/audio-profiles-internal.h"

struct _AudioProfiles {
  guint ref_count;
  gboolean disposed;
  AudioProfileRepository *repository;
  AudioProfilesChanged changed;
  gpointer user_data;
  GDestroyNotify destroy;
};

AudioProfiles *
audio_profiles_ref(AudioProfiles *profiles)
{
  profiles->ref_count++;
  return profiles;
}

void
audio_profiles_unref(AudioProfiles *profiles)
{
  profiles->ref_count--;
  if (profiles->ref_count != 0)
    return;

  audio_profile_repository_free(profiles->repository);
  g_free(profiles);
}

AudioProfiles *
audio_profiles_new(AudioProfilesChanged changed, gpointer user_data, GDestroyNotify destroy)
{
  AudioProfiles *profiles = g_new0(AudioProfiles, 1);

  profiles->ref_count = 1;
  profiles->repository = audio_profile_repository_new();
  profiles->changed = changed;
  profiles->user_data = user_data;
  profiles->destroy = destroy;
  return profiles;
}

void
audio_profiles_free(AudioProfiles *profiles)
{
  if (!profiles)
    return;

  if (!profiles->disposed) {
    profiles->disposed = TRUE;
    profiles->changed = NULL;
    if (profiles->destroy && profiles->user_data)
      profiles->destroy(profiles->user_data);
    profiles->user_data = NULL;
    profiles->destroy = NULL;
  }
  audio_profiles_unref(profiles);
}

gboolean
audio_profiles_is_disposed(AudioProfiles *profiles)
{
  return !profiles || profiles->disposed;
}

AudioProfileRepository *
audio_profiles_repository(AudioProfiles *profiles)
{
  return profiles ? profiles->repository : NULL;
}

void
audio_profiles_notify_changed(AudioProfiles *profiles)
{
  if (profiles && !profiles->disposed && profiles->changed)
    profiles->changed(profiles->user_data);
}
