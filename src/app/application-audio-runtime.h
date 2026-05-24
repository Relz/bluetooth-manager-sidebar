#pragma once

#include "app/application.h"
#include "audio/audio-profiles.h"

typedef struct _ApplicationAudioRuntime ApplicationAudioRuntime;

ApplicationAudioRuntime *application_audio_runtime_new(Application *self);
void application_audio_runtime_free(ApplicationAudioRuntime *runtime);

AudioProfiles *application_audio_runtime_profiles(ApplicationAudioRuntime *runtime);
