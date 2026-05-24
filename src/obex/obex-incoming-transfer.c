#include "obex/obex-incoming-transfer.h"

#include "obex/obex-incoming-storage.h"

static ObexIncomingTransfer *
obex_prepare_incoming_transfer(ObexService *service,
                               ObexTransferDetails *details,
                               GError **error)
{
  return obex_incoming_storage_prepare(service, details, error);
}

ObexIncomingTransfer *
obex_incoming_transfer_prepare(ObexService *service,
                               ObexTransferDetails *details,
                               GError **error)
{
  return obex_prepare_incoming_transfer(service, details, error);
}

gboolean
obex_incoming_transfer_finalize_storage(ObexIncomingTransfer *transfer,
                                        char **out_final,
                                        GError **error)
{
  return obex_incoming_storage_finalize(transfer, out_final, error);
}

void
obex_incoming_transfer_cleanup(ObexIncomingTransfer *transfer)
{
  obex_incoming_storage_cleanup(transfer);
}

gboolean
obex_incoming_transfer_settle_status(ObexIncomingTransfer *transfer,
                                     const char *status,
                                     ObexIncomingTransferSettlement *settlement)
{
  if (settlement)
    *settlement = (ObexIncomingTransferSettlement){ 0 };

  if (!status || !*status || !transfer || !settlement)
    return FALSE;

  if (g_str_equal(status, "complete")) {
    g_autoptr(GError) error = NULL;
    g_autofree char *final = NULL;

    if (obex_incoming_transfer_finalize_storage(transfer, &final, &error)) {
      settlement->status = OBEX_INCOMING_TRANSFER_STATUS_SAVED;
      settlement->final_path = g_steal_pointer(&final);
    } else {
      settlement->status = OBEX_INCOMING_TRANSFER_STATUS_SAVE_ERROR;
      settlement->error_message = g_strdup(error ? error->message : NULL);
    }
    return TRUE;
  }

  if (g_str_equal(status, "error")) {
    obex_incoming_transfer_cleanup(transfer);
    settlement->status = OBEX_INCOMING_TRANSFER_STATUS_FAILED;
    return TRUE;
  }

  settlement->status = OBEX_INCOMING_TRANSFER_STATUS_PENDING;
  return FALSE;
}

void
obex_incoming_transfer_settlement_clear(ObexIncomingTransferSettlement *settlement)
{
  if (!settlement)
    return;

  g_clear_pointer(&settlement->final_path, g_free);
  g_clear_pointer(&settlement->error_message, g_free);
  settlement->status = OBEX_INCOMING_TRANSFER_STATUS_PENDING;
}
