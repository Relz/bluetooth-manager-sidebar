#pragma once

#include <glib.h>

#include "obex/obex-transfer-paths.h"

char *obex_storage_ensure_received_directory(const char *location, GError **error);
gboolean obex_storage_ensure_directory(const char *directory, GError **error);
gboolean obex_storage_finalize_incoming_transfer(const char *staging_path,
                                                     const char *planned_final_path,
                                                     char **out_final_path,
                                                     GError **error);
void obex_storage_cleanup_incoming_transfer(const char *staging_path);
