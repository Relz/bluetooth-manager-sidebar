#pragma once

#include "app/application.h"

GtkWidget *application_settings_bindings_auto_accept_row(Application *self,
                                                         gboolean active,
                                                         gboolean sensitive);
GtkWidget *application_settings_bindings_received_location_row(Application *self,
                                                               const char *location);
