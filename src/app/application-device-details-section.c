#include "app/application-device-settings-sections.h"

#include "app/application-device-presentation.h"
#include "app/application-page-utils.h"

void
application_device_settings_sections_add_details(const DeviceSettingsContext *context,
                                                 AdwPreferencesGroup *group)
{
  static const char *property_names[] = {
    "Alias",
    "Name",
    "Address",
    "AddressType",
    "Icon",
    "Class",
    "Appearance",
    "Paired",
    "Trusted",
    "Blocked",
    "Connected",
    "ServicesResolved",
    "LegacyPairing",
    "UUIDs",
    "Modalias",
  };

  adw_preferences_group_add(group,
                            application_page_selectable_property_row("Object path",
                                                                    context->device_path));

  for (guint i = 0; i < G_N_ELEMENTS(property_names); i++) {
    g_autofree char *display = application_device_presentation_dup_property_display(context->device,
                                                                                    property_names[i]);

    adw_preferences_group_add(group,
                              application_page_selectable_property_row(property_names[i], display));
  }
}
