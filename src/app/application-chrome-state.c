#include "app/application-chrome-state.h"

#include "app/application-bluez-service-access.h"
#include "app/application-device-presentation.h"
#include "app/application-pages.h"
#include "app/application-page-status.h"
#include "bluez/bluez-client-query.h"

void
application_chrome_state_clear(ApplicationChromeState *state)
{
  if (state)
    g_clear_pointer(&state->title, g_free);
}

ApplicationChromeState
application_chrome_state_new(Application *self)
{
  BluezClient *bluez = application_services_bluez_client(self);
  const char *adapter_path = bluez ? bluez_client_primary_adapter(bluez) : NULL;
  ApplicationPage page = application_pages_current_page(self);
  const char *selected_device_path = application_pages_selected_device_path(self);
  BluezProperties *adapter = application_page_adapter_properties(self, adapter_path);
  gboolean powered = bluez_properties_get_bool(adapter, "Powered", FALSE);
  ApplicationChromeState state = {
    .overview = page == APPLICATION_PAGE_OVERVIEW,
    .power_visible = page == APPLICATION_PAGE_OVERVIEW,
    .power_sensitive = adapter != NULL,
    .power_active = powered,
  };

  switch (page) {
    case APPLICATION_PAGE_ADAPTER_INFO:
      state.title = g_strdup("Adapter Information");
      break;
    case APPLICATION_PAGE_SETTINGS:
      state.title = g_strdup("Settings");
      break;
    case APPLICATION_PAGE_DEVICE_SETTINGS:
      state.title = selected_device_path ?
        application_device_presentation_dup_display_name(self, selected_device_path) :
        g_strdup("Device Settings");
      break;
    case APPLICATION_PAGE_OVERVIEW:
    default:
      state.title = g_strdup("Bluetooth");
      break;
  }

  return state;
}
