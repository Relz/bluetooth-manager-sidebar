#include "app/application-audio-profile-model.h"

AudioProfileInfo *
application_audio_profile_model_dup_info(AudioProfiles *audio_profiles, const char *device_path)
{
  AudioProfileInfo *info = audio_profiles ?
    audio_profiles_dup_info(audio_profiles, device_path) :
    audio_profile_info_new();

  if (!audio_profiles && !info->error)
    info->error = g_strdup("Audio profile information has not been loaded.");

  return info;
}

char **
application_audio_profile_model_dup_labels(AudioProfileInfo *info)
{
  char **labels = g_new0(char *, info && info->profiles ? info->profiles->len + 1 : 1);

  for (guint i = 0; info && info->profiles && i < info->profiles->len; i++) {
    AudioProfileOption *option = g_ptr_array_index(info->profiles, i);

    labels[i] = g_strdup(option->label && *option->label ? option->label : option->name && *option->name ? option->name : "Unknown");
  }

  return labels;
}

char **
application_audio_profile_model_dup_profile_names(AudioProfileInfo *info)
{
  char **names = g_new0(char *, info && info->profiles ? info->profiles->len + 1 : 1);

  for (guint i = 0; info && info->profiles && i < info->profiles->len; i++) {
    AudioProfileOption *option = g_ptr_array_index(info->profiles, i);

    names[i] = g_strdup(option->name ? option->name : "");
  }

  return names;
}

guint
application_audio_profile_model_selected_index(AudioProfileInfo *info)
{
  if (!info || !info->profiles || !info->active_profile)
    return 0;

  for (guint i = 0; i < info->profiles->len; i++) {
    AudioProfileOption *option = g_ptr_array_index(info->profiles, i);

    if (g_strcmp0(option->name, info->active_profile) == 0)
      return i;
  }

  return 0;
}
