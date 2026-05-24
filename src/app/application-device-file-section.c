#include "app/application-device-settings-sections.h"

#include "app/application-action-data.h"
#include "app/application-file-actions.h"
#include "app/application-page-utils.h"

void
application_device_settings_sections_add_file_transfer(const DeviceSettingsContext *context,
                                                       AdwPreferencesGroup *group)
{
  GtkWidget *button = NULL;
  GtkWidget *row = application_page_button_row("Send file",
                                               "Send a file to this device using Bluetooth object push.",
                                               "Send file",
                                               FALSE,
                                               &button);

  application_action_set_widget_device_path(button, context->device_path);
  g_signal_connect(button, "clicked", G_CALLBACK(application_actions_on_send_file_clicked), context->self);
  adw_preferences_group_add(group, row);
}
