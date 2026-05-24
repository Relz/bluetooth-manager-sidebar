#include "audio/audio-profiles-internal.h"

#include "audio/audio-profile-messages.h"

AudioProfileOption *
audio_profile_option_new(const char *name, const char *label)
{
  AudioProfileOption *option = g_new0(AudioProfileOption, 1);

  option->name = g_strdup(name ? name : "");
  option->label = g_strdup((label && *label) ? label : (name && *name) ? name : "Unknown");
  return option;
}

static AudioProfileOption *
audio_profile_option_copy(const AudioProfileOption *option)
{
  return option ? audio_profile_option_new(option->name, option->label) : NULL;
}

static void
audio_profile_option_free(AudioProfileOption *option)
{
  if (!option)
    return;

  g_free(option->name);
  g_free(option->label);
  g_free(option);
}

AudioProfileInfo *
audio_profile_info_new(void)
{
  AudioProfileInfo *info = g_new0(AudioProfileInfo, 1);

  info->profiles = g_ptr_array_new_with_free_func((GDestroyNotify)audio_profile_option_free);
  return info;
}

AudioProfileInfo *
audio_profile_info_copy(const AudioProfileInfo *info)
{
  AudioProfileInfo *copy = audio_profile_info_new();

  if (!info)
    return copy;

  copy->loading = info->loading;
  copy->address = g_strdup(info->address);
  copy->card_name = g_strdup(info->card_name);
  copy->active_profile = g_strdup(info->active_profile);
  copy->error = g_strdup(info->error);
  for (guint i = 0; info->profiles && i < info->profiles->len; i++)
    g_ptr_array_add(copy->profiles, audio_profile_option_copy(g_ptr_array_index(info->profiles, i)));

  return copy;
}

void
audio_profile_info_free(AudioProfileInfo *info)
{
  if (!info)
    return;

  g_free(info->address);
  g_free(info->card_name);
  g_free(info->active_profile);
  g_free(info->error);
  g_clear_pointer(&info->profiles, g_ptr_array_unref);
  g_free(info);
}

AudioProfileInfo *
audio_profile_info_new_error(const char *reason, const char *detail)
{
  AudioProfileInfo *info = audio_profile_info_new();

  info->error = audio_profile_load_error_message(reason, detail);
  return info;
}
