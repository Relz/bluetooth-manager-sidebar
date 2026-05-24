#pragma once

#include "settings/settings.h"

SidebarSettings *settings_json_codec_parse_data(const char *contents, gsize length);
char *settings_json_codec_to_data(const SidebarSettings *settings);
