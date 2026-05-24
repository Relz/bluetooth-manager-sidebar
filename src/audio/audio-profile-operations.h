#pragma once

#include "audio/pactl-runner.h"

gboolean audio_profile_operations_load_cards_json(char **stdout_text, PactlError **out_error);
PactlError *audio_profile_operations_set_profile(const char *card_name,
                                                    const char *profile_name);
