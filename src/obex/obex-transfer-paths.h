#pragma once

#include <glib.h>

char *obex_storage_safe_file_name(const char *name);
char *obex_storage_unique_destination(const char *path, GError **error);
char *obex_storage_staging_base_directory(const char *transfer_filename);
