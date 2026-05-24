#pragma once

#include <gtk/gtk.h>

void application_action_set_widget_device_path(GtkWidget *widget, const char *device_path);
const char *application_action_widget_device_path(GtkWidget *widget);
