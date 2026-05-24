#include "audio/audio-profiles.h"

#include "audio/audio-profile-setter.h"
#include "audio/audio-profiles-internal.h"

static void
audio_profiles_update_active_profile(AudioProfiles *profiles,
                                     const char *card_name,
                                     const char *profile_name)
{
  if (audio_profiles_is_disposed(profiles))
    return;

  audio_profile_repository_update_active_profile(audio_profiles_repository(profiles), card_name, profile_name);
  audio_profiles_notify_changed(profiles);
}

static gboolean
audio_profiles_on_profile_set(const char *card_name,
                              const char *profile_name,
                              gboolean success,
                              gpointer user_data)
{
  AudioProfiles *profiles = user_data;

  if (audio_profiles_is_disposed(profiles))
    return FALSE;

  if (success)
    audio_profiles_update_active_profile(profiles, card_name, profile_name);
  else
    audio_profiles_notify_changed(profiles);

  return !audio_profiles_is_disposed(profiles);
}

char *
audio_profiles_set_profile(AudioProfiles *profiles,
                           const char *card_name,
                           const char *profile_name)
{
  gboolean success = FALSE;
  char *message = audio_profile_setter_set_profile(card_name, profile_name, &success);

  if (!audio_profiles_is_disposed(profiles) && success)
    audio_profiles_update_active_profile(profiles, card_name, profile_name);

  return message;
}

void
audio_profiles_set_profile_async(AudioProfiles *profiles,
                                 const char *card_name,
                                 const char *profile_name,
                                 AudioProfileSetFinished callback,
                                 gpointer user_data,
                                 GDestroyNotify destroy)
{
  if (audio_profiles_is_disposed(profiles)) {
    if (destroy && user_data)
      destroy(user_data);
    return;
  }

  audio_profile_setter_set_profile_async(card_name,
                                         profile_name,
                                         audio_profiles_on_profile_set,
                                         audio_profiles_ref(profiles),
                                         (GDestroyNotify)audio_profiles_unref,
                                         callback,
                                         user_data,
                                         destroy);
}
