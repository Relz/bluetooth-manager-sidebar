#include "app/application-services-private.h"

#include "app/application-audio-service-access.h"
#include "app/application-bluez-service-access.h"
#include "app/application-obex-service-access.h"
#include "app/application-private.h"
#include "app/application-settings-service-access.h"

BluezClient *
application_services_bluez_client(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);
  return self->services ? application_bluez_runtime_client(self->services->bluez) : NULL;
}

AudioProfiles *
application_services_audio_profiles(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);
  return self->services ? application_audio_runtime_profiles(self->services->audio) : NULL;
}

ObexService *
application_services_obex_service(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);
  return self->services ? application_obex_runtime_service(self->services->obex) : NULL;
}

SettingsStore *
application_services_settings_store(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);
  return self->services ? application_settings_runtime_store(self->services->settings) : NULL;
}

SidebarSettings *
application_services_settings(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);
  return self->services ? application_settings_runtime_settings(self->services->settings) : NULL;
}
