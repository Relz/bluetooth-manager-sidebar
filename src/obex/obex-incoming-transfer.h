#pragma once

#include "obex/obex-receiver.h"
#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

typedef enum {
  OBEX_INCOMING_TRANSFER_STATUS_PENDING,
  OBEX_INCOMING_TRANSFER_STATUS_SAVED,
  OBEX_INCOMING_TRANSFER_STATUS_SAVE_ERROR,
  OBEX_INCOMING_TRANSFER_STATUS_FAILED,
} ObexIncomingTransferStatus;

typedef struct {
  ObexIncomingTransferStatus status;
  char *final_path;
  char *error_message;
} ObexIncomingTransferSettlement;

ObexIncomingTransfer *obex_incoming_transfer_prepare(ObexService *service,
                                                        ObexTransferDetails *details,
                                                        GError **error);
gboolean obex_incoming_transfer_finalize_storage(ObexIncomingTransfer *transfer,
                                                  char **out_final,
                                                  GError **error);
void obex_incoming_transfer_cleanup(ObexIncomingTransfer *transfer);
gboolean obex_incoming_transfer_settle_status(ObexIncomingTransfer *transfer,
                                              const char *status,
                                              ObexIncomingTransferSettlement *settlement);
void obex_incoming_transfer_settlement_clear(ObexIncomingTransferSettlement *settlement);
