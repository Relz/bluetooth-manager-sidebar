#include "command/command-socket-path.h"

#include <sys/stat.h>

#include "core/runtime-paths.h"

#include <glib/gstdio.h>

char *
command_ipc_dup_socket_path(GError **error)
{
  return paths_runtime_socket_path(error);
}

void
command_ipc_unlink_socket_path(const char *path)
{
  struct stat status;

  if (!path || !*path)
    return;

  if (lstat(path, &status) != 0)
    return;
  if (S_ISSOCK(status.st_mode))
    g_unlink(path);
}
