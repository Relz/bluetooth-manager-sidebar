#include "app/application-audio-runtime.h"

#include "app/application-audio-adapter.h"

struct _ApplicationAudioRuntime {
  AudioProfiles *profiles;
};

ApplicationAudioRuntime *
application_audio_runtime_new(Application *self)
{
  ApplicationAudioRuntime *runtime = g_new0(ApplicationAudioRuntime, 1);

  runtime->profiles = audio_profiles_new(application_audio_adapter_profiles_changed, self, NULL);
  return runtime;
}

void
application_audio_runtime_free(ApplicationAudioRuntime *runtime)
{
  if (!runtime)
    return;

  g_clear_pointer(&runtime->profiles, audio_profiles_free);
  g_free(runtime);
}

AudioProfiles *
application_audio_runtime_profiles(ApplicationAudioRuntime *runtime)
{
  return runtime ? runtime->profiles : NULL;
}
