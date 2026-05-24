#include "app/application-settings-page.h"

#include "app/application-obex-status-section.h"
#include "app/application-settings-bindings.h"
#include "app/application-settings-model.h"
#include "app/application-settings-service-access.h"
#include "app/application-window-access.h"

void
application_settings_page_build(Application *self)
{
  ApplicationSettingsModel model = application_settings_model_new(application_services_settings(self));
  GtkWidget *group = adw_preferences_group_new();

  application_append_content(self, group);
  adw_preferences_group_add(
    ADW_PREFERENCES_GROUP(group),
    application_settings_bindings_auto_accept_row(self,
                                                  model.auto_accept_trusted_files,
                                                  model.settings_available));
  adw_preferences_group_add(
    ADW_PREFERENCES_GROUP(group),
    application_settings_bindings_received_location_row(self, model.received_files_location));

  application_obex_status_section_add(self, ADW_PREFERENCES_GROUP(group));

  application_settings_model_clear(&model);
}
