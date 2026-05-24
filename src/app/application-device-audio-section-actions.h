#pragma once

#include "app/application.h"
#include "audio/audio-profiles.h"

void application_device_audio_section_actions_bind_dropdown(Application *self,
                                                            AudioProfileInfo *info,
                                                            GtkWidget *dropdown);
