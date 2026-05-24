#include "app/application-audio-adapter.h"

#include "app/application-pages.h"

void
application_audio_adapter_profiles_changed(gpointer user_data)
{
  application_schedule_refresh(APPLICATION(user_data), 100, FALSE);
}
