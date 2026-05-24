#include "pairing/pairing-prompt-response.h"

struct _PairingPromptResponse {
  guint ref_count;
  PairingAccept accept;
  PairingEntryAccept entry_accept;
  PairingReject reject;
  gpointer response_data;
  GDestroyNotify response_destroy;
};

PairingPromptResponse *
pairing_prompt_response_new(PairingAccept accept,
                            PairingEntryAccept entry_accept,
                            PairingReject reject,
                            gpointer response_data,
                            GDestroyNotify response_destroy)
{
  PairingPromptResponse *response = g_new0(PairingPromptResponse, 1);

  response->ref_count = 1;
  response->accept = accept;
  response->entry_accept = entry_accept;
  response->reject = reject;
  response->response_data = response_data;
  response->response_destroy = response_destroy;
  return response;
}

PairingPromptResponse *
pairing_prompt_response_ref(PairingPromptResponse *response)
{
  if (response)
    response->ref_count++;
  return response;
}

void
pairing_prompt_response_unref(PairingPromptResponse *response)
{
  if (!response)
    return;

  response->ref_count--;
  if (response->ref_count != 0)
    return;

  if (response->response_destroy && response->response_data)
    response->response_destroy(response->response_data);
  g_free(response);
}

void
pairing_prompt_response_accept(PairingPromptResponse *response, const char *text)
{
  if (!response)
    return;

  if (response->entry_accept)
    response->entry_accept(text ? text : "", response->response_data);
  else if (response->accept)
    response->accept(response->response_data);
}

void
pairing_prompt_response_reject(PairingPromptResponse *response)
{
  if (response && response->reject)
    response->reject(response->response_data);
}
