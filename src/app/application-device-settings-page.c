#include "app/application-device-settings-page.h"

#include "app/application-device-settings-context.h"
#include "app/application-device-settings-sections.h"
#include "app/application-page-utils.h"
#include "app/application-window-access.h"

void
application_device_settings_page_build(Application *self)
{
  DeviceSettingsContext context = application_device_settings_context_new(self);
  GtkWidget *group = adw_preferences_group_new();
  GtkWidget *details_group = NULL;
  GtkWidget *removal_group = NULL;

  application_append_content(self, group);
  if (!context.device_path || !context.device) {
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group),
                              application_page_action_row("Device unavailable",
                                                          "This Bluetooth device is no longer available."));
    return;
  }

  application_device_settings_sections_add_audio(&context, ADW_PREFERENCES_GROUP(group));
  application_device_settings_sections_add_file_transfer(&context, ADW_PREFERENCES_GROUP(group));
  application_device_settings_sections_add_identity(&context, ADW_PREFERENCES_GROUP(group));
  application_device_settings_sections_add_security(&context, ADW_PREFERENCES_GROUP(group));
  application_device_settings_sections_add_battery(&context, ADW_PREFERENCES_GROUP(group));

  details_group = adw_preferences_group_new();
  adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(details_group), "Device Details");
  application_device_settings_sections_add_details(&context, ADW_PREFERENCES_GROUP(details_group));
  application_append_content(self, details_group);

  removal_group = adw_preferences_group_new();
  application_device_settings_sections_add_removal(&context, ADW_PREFERENCES_GROUP(removal_group));
  application_append_content(self, removal_group);
}
