#include "app/application-device-audio-section-model.h"

#include "app/application-audio-profile-model.h"

DeviceAudioSectionModel
application_device_audio_section_model_new(AudioProfiles *audio_profiles, const char *device_path)
{
  return (DeviceAudioSectionModel){
    .info = application_audio_profile_model_dup_info(audio_profiles, device_path),
  };
}

void
application_device_audio_section_model_clear(DeviceAudioSectionModel *model)
{
  g_clear_pointer(&model->info, audio_profile_info_free);
}

gboolean
application_device_audio_section_model_has_choices(const DeviceAudioSectionModel *model)
{
  AudioProfileInfo *info = model ? model->info : NULL;

  return info && info->profiles && info->profiles->len > 0;
}
