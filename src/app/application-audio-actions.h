#pragma once

#include "app/application.h"

gpointer application_actions_audio_profile_dropdown_new(Application *self,
                                                       const char *card_name,
                                                       char **profile_names);
void application_actions_audio_profile_dropdown_free(gpointer data, GClosure *closure);

void application_actions_on_audio_profile_selected(GObject *object,
                                                   GParamSpec *param,
                                                   gpointer user_data);
