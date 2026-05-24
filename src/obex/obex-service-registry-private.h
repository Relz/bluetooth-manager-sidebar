#pragma once

#include "obex/obex-service.h"
#include "obex/obex-transfer-registry.h"

ObexTransferRegistry *obex_service_transfer_registry(ObexService *service);

void obex_service_transfer_registry_shutdown(ObexService *service);
ObexIncomingTransfer *obex_service_lookup_incoming_transfer(ObexService *service,
                                                               const char *transfer_path);
void obex_service_store_incoming_transfer(ObexService *service,
                                          const char *transfer_path,
                                          ObexIncomingTransfer *transfer);
void obex_service_remove_incoming_transfer(ObexService *service,
                                           const char *transfer_path);
ObexOutgoingTransfer *obex_service_lookup_outgoing_transfer(ObexService *service,
                                                               const char *transfer_path);
void obex_service_store_outgoing_transfer(ObexService *service,
                                          const char *transfer_path,
                                          ObexOutgoingTransfer *transfer);
void obex_service_remove_outgoing_transfer(ObexService *service,
                                           const char *transfer_path);
