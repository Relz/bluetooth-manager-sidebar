#include "app/application-services.h"

#include "app/application-private.h"
#include "app/application-services-private.h"

static void
application_services_free(ApplicationServices *services)
{
  if (!services)
    return;

  g_clear_pointer(&services->obex, application_obex_runtime_free);
  g_clear_pointer(&services->bluez, application_bluez_runtime_free);
  g_clear_pointer(&services->audio, application_audio_runtime_free);
  g_clear_pointer(&services->command, application_command_runtime_free);
  g_clear_pointer(&services->settings, application_settings_runtime_free);
  g_free(services);
}

gboolean
application_services_start(Application *self, GError **error)
{
  ApplicationServices *services = NULL;

  g_return_val_if_fail(IS_APPLICATION(self), FALSE);

  if (self->services)
    application_services_stop(self);

  services = g_new0(ApplicationServices, 1);
  services->settings = application_settings_runtime_new();
  services->audio = application_audio_runtime_new(self);
  services->bluez = application_bluez_runtime_new(self);
  services->obex = application_obex_runtime_new(self);
  services->command = application_command_runtime_new();
  self->services = services;

  application_bluez_runtime_start(services->bluez);
  application_obex_runtime_start(services->obex);
  if (!application_command_runtime_start(services->command, self, error)) {
    application_services_stop(self);
    return FALSE;
  }

  return TRUE;
}

gboolean
application_services_ensure_command_server(Application *self, GError **error)
{
  g_return_val_if_fail(IS_APPLICATION(self), FALSE);

  if (!self->services || !self->services->command) {
    g_set_error_literal(error,
                        G_IO_ERROR,
                        G_IO_ERROR_FAILED,
                        "application services are not running");
    return FALSE;
  }

  return application_command_runtime_ensure_healthy(self->services->command, self, error);
}

void
application_services_stop(Application *self)
{
  ApplicationServices *services = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  services = g_steal_pointer(&self->services);
  if (!services)
    return;

  application_services_free(services);
}
