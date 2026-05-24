#include "app/application-settings-commands.h"

#include "app/application-settings-feedback.h"
#include "app/application-settings-service.h"
#include "app/application-settings-service-access.h"

void
application_commands_set_auto_accept_trusted_files(Application *self, gboolean auto_accept)
{
  SettingsStore *settings_store = application_services_settings_store(self);
  SidebarSettings *settings = application_services_settings(self);
  g_autoptr(GError) error = NULL;

  if (!settings_store || !settings)
    return;

  if (!application_settings_service_set_auto_accept_trusted_files(settings_store,
                                                                  settings,
                                                                  auto_accept,
                                                                  &error))
    application_settings_feedback_save_error(self, error);
}

void
application_commands_set_received_files_location(Application *self, const char *location)
{
  SettingsStore *settings_store = application_services_settings_store(self);
  SidebarSettings *settings = application_services_settings(self);
  gboolean changed = FALSE;
  g_autoptr(GError) error = NULL;

  if (!settings_store || !settings)
    return;

  if (!application_settings_service_set_received_files_location(settings_store,
                                                               settings,
                                                                location,
                                                                &changed,
                                                                &error)) {
    application_settings_feedback_save_error(self, error);
    return;
  }

  application_settings_feedback_refresh_if_changed(self, changed);
}
