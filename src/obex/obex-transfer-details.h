#pragma once

#include <gio/gio.h>

typedef struct {
  char *path;
  char *filename;
  char *name;
  guint64 size;
  gboolean has_size;
  char *session_path;
  char *destination;
} ObexTransferDetails;

ObexTransferDetails *obex_transfer_details_read(GDBusConnection *connection,
                                                    const char *transfer_path);
void obex_transfer_details_free(ObexTransferDetails *details);
char *obex_transfer_status_from_properties(GVariant *properties);
