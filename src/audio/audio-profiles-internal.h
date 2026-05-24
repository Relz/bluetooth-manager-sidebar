#pragma once

#include "audio/audio-profile-repository.h"
#include "audio/audio-profiles.h"

AudioProfiles *audio_profiles_ref(AudioProfiles *profiles);
void audio_profiles_unref(AudioProfiles *profiles);
gboolean audio_profiles_is_disposed(AudioProfiles *profiles);
AudioProfileRepository *audio_profiles_repository(AudioProfiles *profiles);
void audio_profiles_notify_changed(AudioProfiles *profiles);

AudioProfileOption *audio_profile_option_new(const char *name, const char *label);
AudioProfileInfo *audio_profile_info_new_error(const char *reason, const char *detail);
