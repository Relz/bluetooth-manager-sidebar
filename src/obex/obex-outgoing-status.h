#pragma once

#include "obex/obex-service.h"

typedef enum {
  OBEX_OUTGOING_TRANSFER_STATUS_PENDING,
  OBEX_OUTGOING_TRANSFER_STATUS_COMPLETE,
  OBEX_OUTGOING_TRANSFER_STATUS_ERROR,
} ObexOutgoingTransferStatus;

ObexOutgoingTransferStatus obex_outgoing_transfer_status_from_text(const char *status);

gboolean obex_outgoing_handle_transfer_status(ObexService *service,
                                              const char *transfer_path,
                                              const char *status);
gboolean obex_outgoing_settle_transfer_status(ObexService *service,
                                              const char *transfer_path,
                                              const char *session_path,
                                              const char *file_name,
                                              const char *device_name,
                                              const char *status);
void obex_outgoing_remove_session_foreach(gpointer key, gpointer value, gpointer user_data);
