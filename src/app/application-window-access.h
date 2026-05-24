#pragma once

#include "app/application.h"

GtkWindow *application_window(Application *self);
GtkWindow *application_ensure_window(Application *self);
void application_append_content(Application *self, GtkWidget *child);
