#include "app/application-audio-commands.h"

#include "app/application-audio-service-access.h"

static void
on_audio_profile_set_finished(const char *message, gpointer user_data)
{
  application_show_toast(APPLICATION(user_data), message);
}

void
application_commands_set_audio_profile(Application *self,
                                       const char *card_name,
                                       const char *profile_name)
{
  AudioProfiles *audio_profiles = application_services_audio_profiles(self);

  if (!audio_profiles || !profile_name)
    return;

  audio_profiles_set_profile_async(audio_profiles,
                                   card_name ? card_name : "",
                                   profile_name,
                                   on_audio_profile_set_finished,
                                   g_object_ref(self),
                                   g_object_unref);
}
