#include "app/application-pairing-adapter.h"

#include "app/application-pairing-device-resolver.h"
#include "app/application-pairing-presenter.h"

static char *
application_pairing_device_name(const char *device_path, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  return application_pairing_device_resolver_dup_name(self, device_path);
}

static void
application_pairing_show_info(const char *message, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  application_pairing_presenter_show_info(self, message);
}

static PairingPromptHandle *
application_pairing_request_confirmation(const char *heading,
                                         const char *body,
                                         PairingAccept accept,
                                         PairingReject reject,
                                         gpointer response_data,
                                         GDestroyNotify response_destroy,
                                         gpointer user_data)
{
  return application_pairing_presenter_request_confirmation(APPLICATION(user_data),
                                                           heading,
                                                           body,
                                                           accept,
                                                           reject,
                                                           response_data,
                                                           response_destroy);
}

static PairingPromptHandle *
application_pairing_request_pin_code(const char *heading,
                                     const char *body,
                                     PairingEntryAccept accept,
                                     PairingReject reject,
                                     gpointer response_data,
                                     GDestroyNotify response_destroy,
                                     gpointer user_data)
{
  return application_pairing_presenter_request_pin_code(APPLICATION(user_data),
                                                       heading,
                                                       body,
                                                       accept,
                                                       reject,
                                                       response_data,
                                                       response_destroy);
}

static PairingPromptHandle *
application_pairing_request_passkey(const char *heading,
                                    const char *body,
                                    PairingEntryAccept accept,
                                    PairingReject reject,
                                    gpointer response_data,
                                    GDestroyNotify response_destroy,
                                    gpointer user_data)
{
  return application_pairing_presenter_request_passkey(APPLICATION(user_data),
                                                      heading,
                                                      body,
                                                      accept,
                                                      reject,
                                                      response_data,
                                                      response_destroy);
}

void
application_pairing_adapter_init_callbacks(Application *self,
                                           BluezPairingAgentCallbacks *callbacks)
{
  *callbacks = (BluezPairingAgentCallbacks){
    .device_name = application_pairing_device_name,
    .show_info = application_pairing_show_info,
    .request_confirmation = application_pairing_request_confirmation,
    .request_pin_code = application_pairing_request_pin_code,
    .request_passkey = application_pairing_request_passkey,
    .user_data = self,
  };
}
