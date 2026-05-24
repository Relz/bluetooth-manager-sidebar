#pragma once

#include "audio/audio-profiles.h"

AudioProfileInfo *audio_profile_parser_parse_cards(const char *stdout_text,
                                                          const char *address);
