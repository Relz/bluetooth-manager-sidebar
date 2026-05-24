#pragma once

#include <gio/gio.h>

typedef struct _ObexIncomingTransfer ObexIncomingTransfer;

ObexIncomingTransfer *obex_receiver_prepare_transfer(const char *received_directory_config,
                                                            const char *name,
                                                            const char *filename,
                                                            GError **error);
void obex_incoming_transfer_free(ObexIncomingTransfer *transfer);
const char *obex_incoming_transfer_staging_path(ObexIncomingTransfer *transfer);
const char *obex_incoming_transfer_final_path(ObexIncomingTransfer *transfer);
gboolean obex_receiver_finalize_transfer(ObexIncomingTransfer *transfer,
                                             char **out_final_path,
                                             GError **error);
void obex_receiver_cleanup_transfer(ObexIncomingTransfer *transfer);
