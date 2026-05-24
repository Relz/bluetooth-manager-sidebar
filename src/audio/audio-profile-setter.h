#pragma once

#include "audio/audio-profiles.h"

typedef gboolean (*AudioProfileSetStateChanged)(const char *card_name,
                                                   const char *profile_name,
                                                   gboolean success,
                                                   gpointer user_data);

char *audio_profile_setter_set_profile(const char *card_name,
                                       const char *profile_name,
                                       gboolean *out_success);
void audio_profile_setter_set_profile_async(const char *card_name,
                                            const char *profile_name,
                                            AudioProfileSetStateChanged state_changed,
                                            gpointer state_user_data,
                                            GDestroyNotify state_destroy,
                                            AudioProfileSetFinished callback,
                                            gpointer callback_user_data,
                                            GDestroyNotify callback_destroy);
