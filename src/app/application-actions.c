#include "app/application-action-data.h"

void
application_action_set_widget_device_path(GtkWidget *widget, const char *device_path)
{
  g_object_set_data_full(G_OBJECT(widget), "bms-device-path", g_strdup(device_path), g_free);
}

const char *
application_action_widget_device_path(GtkWidget *widget)
{
  return g_object_get_data(G_OBJECT(widget), "bms-device-path");
}
