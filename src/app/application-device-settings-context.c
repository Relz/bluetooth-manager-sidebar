#include "app/application-device-settings-context.h"

#include "app/application-audio-service-access.h"
#include "app/application-bluez-service-access.h"
#include "app/application-data-access.h"
#include "app/application-pages.h"

DeviceSettingsContext
application_device_settings_context_new(Application *self)
{
  const char *device_path = application_pages_selected_device_path(self);

  return (DeviceSettingsContext){
    .self = self,
    .device_path = device_path,
    .device = application_device_properties(self, device_path),
    .audio_profiles = application_services_audio_profiles(self),
    .bluez = application_services_bluez_client(self),
  };
}
