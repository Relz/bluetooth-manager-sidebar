#pragma once

#include "audio/audio-profiles.h"

typedef struct {
  AudioProfileInfo *info;
} DeviceAudioSectionModel;

DeviceAudioSectionModel application_device_audio_section_model_new(AudioProfiles *audio_profiles,
                                                                      const char *device_path);
void application_device_audio_section_model_clear(DeviceAudioSectionModel *model);
gboolean application_device_audio_section_model_has_choices(const DeviceAudioSectionModel *model);
