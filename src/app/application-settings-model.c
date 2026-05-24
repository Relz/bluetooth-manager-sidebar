#include "app/application-settings-model.h"

ApplicationSettingsModel
application_settings_model_new(SidebarSettings *settings)
{
  const char *location = sidebar_settings_get_received_files_location(settings);

  return (ApplicationSettingsModel){
    .settings_available = settings != NULL,
    .auto_accept_trusted_files = sidebar_settings_get_auto_accept_trusted_files(settings),
    .received_files_location = g_strdup(location ? location : ""),
  };
}

void
application_settings_model_clear(ApplicationSettingsModel *model)
{
  if (!model)
    return;

  g_clear_pointer(&model->received_files_location, g_free);
}
