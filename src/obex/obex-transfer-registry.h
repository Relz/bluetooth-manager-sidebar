#pragma once

#include <gio/gio.h>

#include "obex/obex-receiver.h"
#include "obex/obex-sender.h"

typedef struct _ObexTransferRegistry ObexTransferRegistry;

ObexTransferRegistry *obex_transfer_registry_new(void);
void obex_transfer_registry_free(ObexTransferRegistry *registry);
void obex_transfer_registry_shutdown(ObexTransferRegistry *registry,
                                     GHFunc incoming_cleanup,
                                     GHFunc outgoing_cleanup,
                                     gpointer user_data);

ObexIncomingTransfer *obex_transfer_registry_lookup_incoming(ObexTransferRegistry *registry,
                                                                const char *transfer_path);
void obex_transfer_registry_store_incoming(ObexTransferRegistry *registry,
                                           const char *transfer_path,
                                           ObexIncomingTransfer *transfer);
void obex_transfer_registry_remove_incoming(ObexTransferRegistry *registry,
                                            const char *transfer_path);

ObexOutgoingTransfer *obex_transfer_registry_lookup_outgoing(ObexTransferRegistry *registry,
                                                                const char *transfer_path);
void obex_transfer_registry_store_outgoing(ObexTransferRegistry *registry,
                                           const char *transfer_path,
                                           ObexOutgoingTransfer *transfer);
void obex_transfer_registry_remove_outgoing(ObexTransferRegistry *registry,
                                            const char *transfer_path);
