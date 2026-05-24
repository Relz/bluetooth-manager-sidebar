#include "app/application-pairing-presenter.h"

#include "app/application-window-access.h"
#include "pairing/pairing-prompt-presenter.h"

static PairingPromptHandle *
application_pairing_presenter_present_prompt(Application *self,
                                             const char *heading,
                                             const char *body,
                                             GtkWidget *extra_child,
                                             PairingAccept accept,
                                             PairingEntryAccept entry_accept,
                                             PairingReject reject,
                                             gpointer response_data,
                                             GDestroyNotify response_destroy)
{
  GtkWindow *window = application_ensure_window(self);

  return pairing_prompt_present(window,
                                heading,
                                body,
                                extra_child,
                                accept,
                                entry_accept,
                                reject,
                                response_data,
                                response_destroy);
}

static PairingPromptHandle *
application_pairing_presenter_request_entry(Application *self,
                                            const char *heading,
                                            const char *body,
                                            PairingEntryAccept accept,
                                            PairingReject reject,
                                            gpointer response_data,
                                            GDestroyNotify response_destroy,
                                            gboolean passkey)
{
  return pairing_prompt_present_entry(application_ensure_window(self),
                                      heading,
                                      body,
                                      passkey,
                                      accept,
                                      reject,
                                      response_data,
                                      response_destroy);
}

void
application_pairing_presenter_show_info(Application *self, const char *message)
{
  application_show_sidebar(self);
  application_show_toast(self, message);
}

PairingPromptHandle *
application_pairing_presenter_request_confirmation(Application *self,
                                                  const char *heading,
                                                  const char *body,
                                                  PairingAccept accept,
                                                  PairingReject reject,
                                                  gpointer response_data,
                                                  GDestroyNotify response_destroy)
{
  return application_pairing_presenter_present_prompt(self,
                                                      heading,
                                                      body,
                                                      NULL,
                                                      accept,
                                                      NULL,
                                                      reject,
                                                      response_data,
                                                      response_destroy);
}

PairingPromptHandle *
application_pairing_presenter_request_pin_code(Application *self,
                                              const char *heading,
                                              const char *body,
                                              PairingEntryAccept accept,
                                              PairingReject reject,
                                              gpointer response_data,
                                              GDestroyNotify response_destroy)
{
  return application_pairing_presenter_request_entry(self,
                                                     heading,
                                                     body,
                                                     accept,
                                                     reject,
                                                     response_data,
                                                     response_destroy,
                                                     FALSE);
}

PairingPromptHandle *
application_pairing_presenter_request_passkey(Application *self,
                                             const char *heading,
                                             const char *body,
                                             PairingEntryAccept accept,
                                             PairingReject reject,
                                             gpointer response_data,
                                             GDestroyNotify response_destroy)
{
  return application_pairing_presenter_request_entry(self,
                                                     heading,
                                                     body,
                                                     accept,
                                                     reject,
                                                     response_data,
                                                     response_destroy,
                                                     TRUE);
}
