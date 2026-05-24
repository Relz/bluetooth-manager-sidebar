#pragma once

#include <glib.h>

char *format_size(guint64 size);
char *display_dup_safe_text(const char *value, const char *fallback);
char *device_display_name(const char *alias,
                               const char *name,
                               const char *address,
                               const char *fallback);
