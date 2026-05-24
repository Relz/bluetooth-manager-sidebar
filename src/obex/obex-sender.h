#pragma once

#include <gio/gio.h>

typedef struct _ObexOutgoingTransfer ObexOutgoingTransfer;

ObexOutgoingTransfer *obex_sender_transfer_new(const char *session_path,
                                                  const char *file_name,
                                                  const char *device_name);
void obex_outgoing_transfer_free(ObexOutgoingTransfer *transfer);
const char *obex_outgoing_transfer_session_path(ObexOutgoingTransfer *transfer);
const char *obex_outgoing_transfer_file_name(ObexOutgoingTransfer *transfer);
const char *obex_outgoing_transfer_device_name(ObexOutgoingTransfer *transfer);
gboolean obex_sender_validate_file(const char *file_path, char **out_message);
void obex_sender_add_session_options(GVariantBuilder *builder);
