#pragma once

#include "obex/obex-receiver.h"
#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

ObexIncomingTransfer *obex_incoming_storage_prepare(ObexService *service,
                                                       const ObexTransferDetails *details,
                                                       GError **error);
gboolean obex_incoming_storage_finalize(ObexIncomingTransfer *transfer,
                                        char **out_final_path,
                                        GError **error);
void obex_incoming_storage_cleanup(ObexIncomingTransfer *transfer);
