#pragma once

#include "audio/audio-profile-load-task.h"
#include "audio/audio-profile-repository.h"

gboolean audio_profile_loader_start(AudioProfileRepository *repository,
                                    const char *device_path,
                                    const char *address,
                                    AudioProfileLoadTaskFinished callback,
                                    gpointer user_data,
                                    GDestroyNotify destroy);
