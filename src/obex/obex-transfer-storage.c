#include "obex/obex-transfer-storage.h"

#include <errno.h>

#include <gio/gio.h>
#include <glib/gstdio.h>

#include "obex/obex-received-directory.h"

char *
obex_storage_ensure_received_directory(const char *location, GError **error)
{
  char *directory = obex_received_directory_expand(location);

  if (!obex_storage_ensure_directory(directory, error)) {
    g_free(directory);
    return NULL;
  }

  if (!g_file_test(directory, G_FILE_TEST_IS_DIR)) {
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOTDIR, "%s is not a directory", directory);
    g_free(directory);
    return NULL;
  }

  return directory;
}

gboolean
obex_storage_ensure_directory(const char *directory, GError **error)
{
  if (g_mkdir_with_parents(directory, 0700) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_FILE_ERROR,
                g_file_error_from_errno(saved_errno),
                "%s",
                g_strerror(saved_errno));
    return FALSE;
  }

  return TRUE;
}

gboolean
obex_storage_finalize_incoming_transfer(const char *staging_path,
                                            const char *planned_final_path,
                                            char **out_final_path,
                                            GError **error)
{
  g_autofree char *final = obex_storage_unique_destination(planned_final_path, error);
  g_autoptr(GFile) source = NULL;
  g_autoptr(GFile) destination = NULL;

  if (!final)
    return FALSE;

  source = g_file_new_for_path(staging_path);
  destination = g_file_new_for_path(final);
  if (!g_file_move(source, destination, G_FILE_COPY_NONE, NULL, NULL, NULL, error))
    return FALSE;

  if (out_final_path)
    *out_final_path = g_steal_pointer(&final);
  return TRUE;
}

void
obex_storage_cleanup_incoming_transfer(const char *staging_path)
{
  if (staging_path)
    g_unlink(staging_path);
}
