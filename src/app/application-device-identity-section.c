#include "app/application-device-settings-sections.h"

#include "app/application-action-data.h"
#include "app/application-device-actions.h"
#include "app/application-page-utils.h"
#include "bluez/bluez-properties.h"

typedef struct {
  char *name;
} DeviceIdentitySectionModel;

static DeviceIdentitySectionModel
application_device_identity_section_model_new(BluezProperties *device)
{
  g_autofree char *alias = bluez_properties_dup_string(device, "Alias");
  g_autofree char *name = bluez_properties_dup_string(device, "Name");

  return (DeviceIdentitySectionModel){
    .name = g_strdup(alias ? alias : name ? name : ""),
  };
}

static void
application_device_identity_section_model_clear(DeviceIdentitySectionModel *model)
{
  g_clear_pointer(&model->name, g_free);
}

void
application_device_settings_sections_add_identity(const DeviceSettingsContext *context,
                                                  AdwPreferencesGroup *group)
{
  DeviceIdentitySectionModel model = application_device_identity_section_model_new(context->device);
  GtkWidget *name_entry = NULL;
  GtkWidget *name_row = application_page_labeled_entry_row("Device name",
                                                           model.name,
                                                           "Device name",
                                                           &name_entry);
  application_action_set_widget_device_path(name_entry, context->device_path);
  g_signal_connect(name_entry, "activate", G_CALLBACK(application_actions_on_alias_entry_activated), context->self);
  g_signal_connect(name_entry,
                   "notify::has-focus",
                   G_CALLBACK(application_actions_on_alias_entry_focus_changed),
                   context->self);
  adw_preferences_group_add(group, name_row);
  application_device_identity_section_model_clear(&model);
}
