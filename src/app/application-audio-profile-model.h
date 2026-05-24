#pragma once

#include "audio/audio-profiles.h"

AudioProfileInfo *application_audio_profile_model_dup_info(AudioProfiles *audio_profiles,
                                                              const char *device_path);
char **application_audio_profile_model_dup_labels(AudioProfileInfo *info);
char **application_audio_profile_model_dup_profile_names(AudioProfileInfo *info);
guint application_audio_profile_model_selected_index(AudioProfileInfo *info);
