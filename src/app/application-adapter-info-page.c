#include "app/application-adapter-info-page.h"

#include "app/application-adapter-info-model.h"
#include "app/application-bluez-service-access.h"
#include "app/application-device-presentation.h"
#include "app/application-window-access.h"
#include "app/application-page-utils.h"

void
application_adapter_info_page_build(Application *self)
{
  ApplicationAdapterInfoModel model = application_adapter_info_model_new(
    self,
    application_services_bluez_client(self));
  GtkWidget *group = adw_preferences_group_new();
  static const char *property_names[] = {
    "Alias",
    "Name",
    "Address",
    "AddressType",
    "Powered",
    "PowerState",
    "Discoverable",
    "Pairable",
    "Discovering",
    "Roles",
    "UUIDs",
  };

  adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(group), "Adapter");
  application_append_content(self, group);

  if (!model.adapter_path || !model.adapter) {
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group),
                              application_page_action_row("No adapter", model.status_message));
    application_adapter_info_model_clear(&model);
    return;
  }

  for (guint i = 0; i < G_N_ELEMENTS(property_names); i++) {
    g_autofree char *display = application_device_presentation_dup_property_display(model.adapter, property_names[i]);

    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group),
                              application_page_selectable_property_row(property_names[i], display));
  }

  application_adapter_info_model_clear(&model);
}
