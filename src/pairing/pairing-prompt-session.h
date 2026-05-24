#pragma once

#include <glib.h>

typedef struct _PairingPromptHandle PairingPromptHandle;

typedef void (*PairingPromptClose)(gpointer handle_data);

PairingPromptHandle *pairing_prompt_handle_new(gpointer handle_data,
                                                      PairingPromptClose close,
                                                      GDestroyNotify destroy);
void pairing_prompt_handle_close(PairingPromptHandle *handle);
void pairing_prompt_handle_free(PairingPromptHandle *handle);

typedef void (*PairingAccept)(gpointer user_data);
typedef void (*PairingReject)(gpointer user_data);
typedef void (*PairingEntryAccept)(const char *text, gpointer user_data);

typedef gpointer (*PairingResponseRef)(gpointer response);
typedef void (*PairingResponseReject)(gpointer response, const char *message);

typedef struct {
  PairingPromptHandle *active_prompt;
  gpointer active_response;
  GDestroyNotify response_unref;
} PairingPromptSession;

void pairing_prompt_session_init(PairingPromptSession *session);
void pairing_prompt_session_clear(PairingPromptSession *session,
                                      gboolean reject,
                                      const char *message,
                                      PairingResponseReject reject_response);
void pairing_prompt_session_replace_response(PairingPromptSession *session,
                                                 gpointer response,
                                                 PairingResponseRef response_ref,
                                                 GDestroyNotify response_unref,
                                                 const char *message,
                                                 PairingResponseReject reject_response);
gboolean pairing_prompt_session_take_response(PairingPromptSession *session,
                                                  gpointer response,
                                                  PairingPromptHandle **prompt);
gboolean pairing_prompt_session_has_response(PairingPromptSession *session,
                                                 gpointer response);
void pairing_prompt_session_set_prompt(PairingPromptSession *session,
                                           PairingPromptHandle *prompt);
