#pragma once

#include <glib.h>

#define TARGET_OUTPUT_MAX_CHARS 128

char *target_output_normalize(const char *output_name);
char *target_output_from_environment(void);
