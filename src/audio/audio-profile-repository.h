#pragma once

#include "audio/audio-profiles.h"

typedef struct _AudioProfileRepository AudioProfileRepository;

AudioProfileRepository *audio_profile_repository_new(void);
void audio_profile_repository_free(AudioProfileRepository *repository);
AudioProfileInfo *audio_profile_repository_lookup(AudioProfileRepository *repository,
                                                     const char *device_path);
AudioProfileInfo *audio_profile_repository_dup_info(AudioProfileRepository *repository,
                                                       const char *device_path);
void audio_profile_repository_replace(AudioProfileRepository *repository,
                                      const char *device_path,
                                      AudioProfileInfo *info);
void audio_profile_repository_update_active_profile(AudioProfileRepository *repository,
                                                    const char *card_name,
                                                    const char *profile_name);
