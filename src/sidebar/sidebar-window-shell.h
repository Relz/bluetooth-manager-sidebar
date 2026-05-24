#pragma once

#include <adwaita.h>

gboolean sidebar_window_shell_is_supported(void);
GtkWindow *sidebar_window_shell_create_application_window(GtkApplication *application);
void sidebar_window_shell_initialize(GtkWindow *window);
void sidebar_window_shell_configure(GtkWindow *window, const char *target_output_name);
