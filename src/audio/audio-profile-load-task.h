#pragma once

#include "audio/audio-profiles.h"

typedef void (*AudioProfileLoadTaskFinished)(const char *device_path,
                                                const char *address,
                                                AudioProfileInfo *info,
                                                gpointer user_data);

void audio_profile_load_task_start(const char *device_path,
                                   const char *address,
                                   AudioProfileLoadTaskFinished callback,
                                   gpointer user_data,
                                   GDestroyNotify destroy);
