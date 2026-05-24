#include "app/application-audio-page-loader.h"

#include "app/application-audio-service-access.h"
#include "app/application-data-access.h"
#include "app/application-pages.h"

static void
application_audio_page_loader_load_device(Application *self, const char *device_path)
{
  AudioProfiles *audio_profiles = application_services_audio_profiles(self);
  BluezProperties *device;
  g_autofree char *address = NULL;

  if (!audio_profiles || !device_path)
    return;

  device = application_device_properties(self, device_path);
  address = bluez_properties_dup_string(device, "Address");
  audio_profiles_load(audio_profiles, device_path, address);
}

void
application_audio_page_loader_load_selected_device(Application *self)
{
  if (!application_pages_is_device_settings(self))
    return;

  application_audio_page_loader_load_device(
    self,
    application_pages_selected_device_path(self));
}
