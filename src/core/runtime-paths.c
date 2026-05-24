#include "core/runtime-paths.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include <glib/gstdio.h>

#include "core/config.h"

static char *
runtime_dir(void)
{
  const char *xdg_runtime_dir = g_getenv("XDG_RUNTIME_DIR");

  if (xdg_runtime_dir && *xdg_runtime_dir)
    return g_strdup(xdg_runtime_dir);

  return g_strdup_printf("/tmp/%s-%lu", COMMAND_NAME, (unsigned long)getuid());
}

static gboolean
validate_runtime_dir(const char *dir, const char *label, GError **error)
{
  struct stat status;
  uid_t uid = getuid();
  mode_t mode;

  if (lstat(dir, &status) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_FILE_ERROR,
                g_file_error_from_errno(saved_errno),
                "Unable to inspect %s %s: %s",
                label,
                dir,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (S_ISLNK(status.st_mode)) {
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "refusing %s %s: path is a symlink", label, dir);
    return FALSE;
  }
  if (!S_ISDIR(status.st_mode)) {
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, "refusing %s %s: path is not a directory", label, dir);
    return FALSE;
  }
  if (status.st_uid != uid) {
    g_set_error(error,
                G_FILE_ERROR,
                G_FILE_ERROR_FAILED,
                "refusing %s %s: owned by uid %lu, expected %lu",
                label,
                dir,
                (unsigned long)status.st_uid,
                (unsigned long)uid);
    return FALSE;
  }

  mode = status.st_mode & 0777;
  if ((mode & 0077) != 0) {
    g_set_error(error,
                G_FILE_ERROR,
                G_FILE_ERROR_FAILED,
                "refusing %s %s: permissions are %04o, expected no group/world access",
                label,
                dir,
                mode);
    return FALSE;
  }

  return TRUE;
}

static gboolean
paths_ensure_runtime_dir(GError **error)
{
  g_autofree char *dir = runtime_dir();
  const char *xdg_runtime_dir = g_getenv("XDG_RUNTIME_DIR");
  const char *label = xdg_runtime_dir && *xdg_runtime_dir ? "XDG_RUNTIME_DIR" : "fallback runtime directory";

  if (!g_path_is_absolute(dir)) {
    g_set_error(error,
                G_FILE_ERROR,
                G_FILE_ERROR_FAILED,
                "refusing %s %s: path must be absolute",
                label,
                dir);
    return FALSE;
  }

  if (g_mkdir_with_parents(dir, 0700) == 0)
    return validate_runtime_dir(dir, label, error);

  int saved_errno = errno;
  g_set_error(error,
              G_FILE_ERROR,
              g_file_error_from_errno(saved_errno),
              "Unable to create runtime directory %s: %s",
              dir,
              g_strerror(saved_errno));
  return FALSE;
}

char *
paths_runtime_socket_path(GError **error)
{
  g_autofree char *dir = runtime_dir();

  if (!paths_ensure_runtime_dir(error))
    return NULL;

  return g_build_filename(dir, SOCKET_NAME, NULL);
}
