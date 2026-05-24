#include "app/application-device-settings-sections.h"

#include "app/application-page-utils.h"
#include "bluez/bluez-client-query.h"

typedef struct {
  char *subtitle;
} DeviceBatterySectionModel;

static DeviceBatterySectionModel
application_device_battery_section_model_new(BluezClient *bluez, const char *device_path)
{
  gboolean battery_found = FALSE;
  int battery = bluez ? bluez_client_device_battery(bluez, device_path, &battery_found) : 0;

  return (DeviceBatterySectionModel){
    .subtitle = battery_found ? g_strdup_printf("%d%%", battery) : g_strdup("Unknown"),
  };
}

static void
application_device_battery_section_model_clear(DeviceBatterySectionModel *model)
{
  g_clear_pointer(&model->subtitle, g_free);
}

void
application_device_settings_sections_add_battery(const DeviceSettingsContext *context,
                                                 AdwPreferencesGroup *group)
{
  DeviceBatterySectionModel model = application_device_battery_section_model_new(context->bluez,
                                                                                    context->device_path);

  adw_preferences_group_add(group, application_page_action_row("Battery level", model.subtitle));
  application_device_battery_section_model_clear(&model);
}
