#pragma once

#include "audio/audio-profiles.h"

typedef struct _AudioProfileCache AudioProfileCache;

AudioProfileCache *audio_profile_cache_new(void);
void audio_profile_cache_free(AudioProfileCache *cache);
AudioProfileInfo *audio_profile_cache_lookup(AudioProfileCache *cache,
                                                    const char *device_path);
AudioProfileInfo *audio_profile_cache_dup_info(AudioProfileCache *cache,
                                                      const char *device_path);
void audio_profile_cache_replace(AudioProfileCache *cache,
                                     const char *device_path,
                                     AudioProfileInfo *info);
void audio_profile_cache_update_active_profile(AudioProfileCache *cache,
                                                   const char *card_name,
                                                   const char *profile_name);
