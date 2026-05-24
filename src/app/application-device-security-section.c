#include "app/application-device-settings-sections.h"

#include "app/application-action-data.h"
#include "app/application-device-actions.h"
#include "app/application-page-utils.h"
#include "bluez/bluez-properties.h"

typedef struct {
  gboolean trusted;
  gboolean blocked;
} DeviceSecuritySectionModel;

static DeviceSecuritySectionModel
application_device_security_section_model_new(BluezProperties *device)
{
  return (DeviceSecuritySectionModel){
    .trusted = bluez_properties_get_bool(device, "Trusted", FALSE),
    .blocked = bluez_properties_get_bool(device, "Blocked", FALSE),
  };
}

void
application_device_settings_sections_add_security(const DeviceSettingsContext *context,
                                                  AdwPreferencesGroup *group)
{
  DeviceSecuritySectionModel model = application_device_security_section_model_new(context->device);
  GtkWidget *trusted_switch = NULL;
  GtkWidget *blocked_switch = NULL;
  GtkWidget *trusted_row = application_page_switch_row("Trusted",
                                                       model.trusted,
                                                       TRUE,
                                                       &trusted_switch);
  GtkWidget *blocked_row = application_page_switch_row("Blocked",
                                                       model.blocked,
                                                       TRUE,
                                                       &blocked_switch);

  application_action_set_widget_device_path(trusted_switch, context->device_path);
  application_action_set_widget_device_path(blocked_switch, context->device_path);
  g_signal_connect(trusted_switch,
                   "notify::active",
                   G_CALLBACK(application_actions_on_device_trusted_changed),
                   context->self);
  g_signal_connect(blocked_switch,
                   "notify::active",
                   G_CALLBACK(application_actions_on_device_blocked_changed),
                   context->self);
  adw_preferences_group_add(group, trusted_row);
  adw_preferences_group_add(group, blocked_row);
}
