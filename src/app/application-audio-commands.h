#pragma once

#include "app/application.h"

void application_commands_set_audio_profile(Application *self,
                                            const char *card_name,
                                            const char *profile_name);
