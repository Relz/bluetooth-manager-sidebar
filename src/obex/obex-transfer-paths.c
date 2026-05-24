#include "obex/obex-transfer-paths.h"

#include <string.h>

#include <gio/gio.h>

char *
obex_storage_safe_file_name(const char *name)
{
  g_autofree char *basename = NULL;

  if (name && *name)
    basename = g_path_get_basename(name);

  if (!basename || !*basename || g_str_equal(basename, ".") || g_str_equal(basename, ".."))
    return g_strdup("Bluetooth file");

  return g_steal_pointer(&basename);
}

static gboolean
obex_storage_path_exists_or_symlink(const char *path)
{
  return g_file_test(path, G_FILE_TEST_EXISTS) || g_file_test(path, G_FILE_TEST_IS_SYMLINK);
}

char *
obex_storage_unique_destination(const char *path, GError **error)
{
  g_autofree char *dir = g_path_get_dirname(path);
  g_autofree char *base = g_path_get_basename(path);
  char *dot = strrchr(base, '.');
  g_autofree char *stem = NULL;
  const char *suffix = "";

  if (!obex_storage_path_exists_or_symlink(path))
    return g_strdup(path);

  if (dot && dot != base) {
    stem = g_strndup(base, (gsize)(dot - base));
    suffix = dot;
  } else {
    stem = g_strdup(base);
  }

  for (guint i = 1; i <= 999; i++) {
    g_autofree char *candidate_base = g_strdup_printf("%s (%u)%s", stem, i, suffix);
    char *candidate = g_build_filename(dir, candidate_base, NULL);

    if (!obex_storage_path_exists_or_symlink(candidate))
      return candidate;
    g_free(candidate);
  }

  g_set_error(error,
              G_FILE_ERROR,
              G_FILE_ERROR_EXIST,
              "Unable to choose a unique destination for %s",
              path);
  return NULL;
}

char *
obex_storage_staging_base_directory(const char *transfer_filename)
{
  if (transfer_filename && *transfer_filename)
    return g_path_get_dirname(transfer_filename);

  return g_build_filename(g_get_home_dir(), ".cache", "obexd", NULL);
}
