#include "pairing/pairing-prompt-session.h"

struct _PairingPromptHandle {
  gpointer handle_data;
  PairingPromptClose close;
  GDestroyNotify destroy;
  gboolean closed;
};

PairingPromptHandle *
pairing_prompt_handle_new(gpointer handle_data,
                              PairingPromptClose close,
                              GDestroyNotify destroy)
{
  PairingPromptHandle *handle = g_new0(PairingPromptHandle, 1);

  handle->handle_data = handle_data;
  handle->close = close;
  handle->destroy = destroy;

  return handle;
}

void
pairing_prompt_handle_close(PairingPromptHandle *handle)
{
  if (!handle || handle->closed)
    return;

  handle->closed = TRUE;
  if (handle->close)
    handle->close(handle->handle_data);
}

void
pairing_prompt_handle_free(PairingPromptHandle *handle)
{
  if (!handle)
    return;

  if (handle->destroy && handle->handle_data)
    handle->destroy(handle->handle_data);
  g_free(handle);
}

void
pairing_prompt_session_init(PairingPromptSession *session)
{
  if (session)
    *session = (PairingPromptSession){ 0 };
}

void
pairing_prompt_session_clear(PairingPromptSession *session,
                                 gboolean reject,
                                 const char *message,
                                 PairingResponseReject reject_response)
{
  PairingPromptHandle *prompt = NULL;
  gpointer response = NULL;
  GDestroyNotify response_unref = NULL;

  if (!session)
    return;

  prompt = session->active_prompt;
  response = session->active_response;
  response_unref = session->response_unref;
  session->active_prompt = NULL;
  session->active_response = NULL;
  session->response_unref = NULL;

  if (prompt) {
    pairing_prompt_handle_close(prompt);
    pairing_prompt_handle_free(prompt);
  }

  if (reject && response && reject_response)
    reject_response(response, message);
  if (response && response_unref)
    response_unref(response);
}

void
pairing_prompt_session_replace_response(PairingPromptSession *session,
                                            gpointer response,
                                            PairingResponseRef response_ref,
                                            GDestroyNotify response_unref,
                                            const char *message,
                                            PairingResponseReject reject_response)
{
  if (!session)
    return;

  pairing_prompt_session_clear(session, TRUE, message, reject_response);
  session->active_response = response_ref ? response_ref(response) : response;
  session->response_unref = response_unref;
}

gboolean
pairing_prompt_session_take_response(PairingPromptSession *session,
                                         gpointer response,
                                         PairingPromptHandle **prompt)
{
  if (!session || session->active_response != response)
    return FALSE;

  if (prompt)
    *prompt = session->active_prompt;
  else
    pairing_prompt_handle_free(session->active_prompt);

  session->active_prompt = NULL;
  session->active_response = NULL;
  session->response_unref = NULL;
  return TRUE;
}

gboolean
pairing_prompt_session_has_response(PairingPromptSession *session, gpointer response)
{
  return session && session->active_response == response;
}

void
pairing_prompt_session_set_prompt(PairingPromptSession *session,
                                      PairingPromptHandle *prompt)
{
  if (session)
    session->active_prompt = prompt;
}
