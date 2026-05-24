#pragma once

#include "audio/pactl-runner.h"

typedef void (*AudioProfileSetTaskFinished)(const char *card_name,
                                               const char *profile_name,
                                               PactlError *error,
                                               gpointer user_data);

void audio_profile_set_task_start(const char *card_name,
                                  const char *profile_name,
                                  AudioProfileSetTaskFinished callback,
                                  gpointer user_data,
                                  GDestroyNotify destroy);
