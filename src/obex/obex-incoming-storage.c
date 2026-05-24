#include "obex/obex-incoming-storage.h"

#include "obex/obex-service-policy-private.h"

ObexIncomingTransfer *
obex_incoming_storage_prepare(ObexService *service,
                              const ObexTransferDetails *details,
                              GError **error)
{
  g_autofree char *configured = obex_service_dup_received_directory(service);

  return obex_receiver_prepare_transfer(configured,
                                        details ? details->name : NULL,
                                        details ? details->filename : NULL,
                                        error);
}

gboolean
obex_incoming_storage_finalize(ObexIncomingTransfer *transfer,
                               char **out_final_path,
                               GError **error)
{
  return obex_receiver_finalize_transfer(transfer, out_final_path, error);
}

void
obex_incoming_storage_cleanup(ObexIncomingTransfer *transfer)
{
  obex_receiver_cleanup_transfer(transfer);
}
