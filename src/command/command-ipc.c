#include "command/command-ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gio/gunixsocketaddress.h>
#include <glib/gstdio.h>

#include "app/app-metadata.h"
#include "command/command-socket.h"

#define COMMAND_SERVER_STARTUP_LOCK_TIMEOUT_SECONDS 3.0
#define COMMAND_SERVER_STARTUP_LOCK_RETRY_SECONDS 0.05

struct _CommandServer {
  char *path;
  char *startup_lock_path;
  GSocket *socket;
  GThread *thread;
  CommandHandler handler;
  gpointer user_data;
  GDestroyNotify destroy;
  gint ref_count;
  int startup_lock_fd;
  dev_t bound_dev;
  ino_t bound_ino;
  gboolean has_bound_socket_id;
  gint closed;
};

typedef struct {
  CommandServer *server;
  SidebarCommand command;
  char *target_output_name;
} CommandDispatch;

static gboolean command_server_bind_socket(CommandServer *server, GError **error);
static gboolean command_server_validate_socket_directory(CommandServer *server, GError **error);
static gboolean command_server_acquire_startup_lock(CommandServer *server, GError **error);
static void command_server_release_startup_lock(CommandServer *server);
static void command_server_unlink_bound_socket_path(CommandServer *server);
static gpointer command_server_thread(gpointer user_data);
static gboolean command_server_dispatch_cb(gpointer user_data);
static gboolean command_server_dispatch(CommandServer *server,
                                        SidebarCommand command,
                                        char **target_output_name);
static CommandServer *command_server_ref(CommandServer *server);
static void command_server_unref(CommandServer *server);

static gboolean
command_server_dispatch_cb(gpointer user_data)
{
  CommandDispatch *dispatch = user_data;
  CommandServer *server = dispatch->server;

  if (!g_atomic_int_get(&server->closed) && server->handler)
    server->handler(dispatch->command, dispatch->target_output_name, server->user_data);

  g_free(dispatch->target_output_name);
  command_server_unref(server);
  g_free(dispatch);
  return G_SOURCE_REMOVE;
}

static gboolean
command_server_dispatch(CommandServer *server,
                        SidebarCommand command,
                        char **target_output_name)
{
  CommandDispatch *dispatch;

  if (!server->handler)
    return TRUE;

  dispatch = g_new0(CommandDispatch, 1);
  dispatch->server = command_server_ref(server);
  dispatch->command = command;
  dispatch->target_output_name = g_steal_pointer(target_output_name);

  if (g_idle_add(command_server_dispatch_cb, dispatch) != 0)
    return TRUE;

  g_free(dispatch->target_output_name);
  command_server_unref(dispatch->server);
  g_free(dispatch);
  return FALSE;
}

static void
command_server_handle_connection(CommandServer *server, GSocket *connection)
{
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  g_autofree char *target_output_name = NULL;

  if (!command_socket_read_command(connection, &command, &target_output_name))
    return;

  if (command == SIDEBAR_COMMAND_PING || command_server_dispatch(server, command, &target_output_name))
    command_socket_send_success(connection, command);
}

static gpointer
command_server_thread(gpointer user_data)
{
  CommandServer *server = user_data;

  while (!g_atomic_int_get(&server->closed)) {
    g_autoptr(GError) error = NULL;
    g_autoptr(GSocket) connection = g_socket_accept(server->socket, NULL, &error);

    if (!connection) {
      if (g_atomic_int_get(&server->closed))
        break;
      if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT) ||
          g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK) ||
          g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        continue;
      continue;
    }

    command_server_handle_connection(server, connection);
  }

  return NULL;
}

CommandServer *
command_server_new(CommandHandler handler,
                       gpointer user_data,
                       GDestroyNotify destroy,
                       GError **error)
{
  g_autofree char *path = NULL;
  CommandServer *server;

  path = command_ipc_dup_socket_path(error);
  if (!path)
    return NULL;

  server = g_new0(CommandServer, 1);
  server->path = g_strdup(path);
  server->startup_lock_path = g_strdup_printf("%s.startup.lock", path);
  server->handler = handler;
  server->user_data = user_data;
  server->destroy = destroy;
  server->ref_count = 1;
  server->startup_lock_fd = -1;

  server->socket = g_socket_new(G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, error);
  if (!server->socket)
    goto fail;

  if (!command_server_bind_socket(server, error))
    goto fail;

  server->thread = g_thread_new("bm-command-server", command_server_thread, server);

  return server;

fail:
  command_server_free(server);
  return NULL;
}

static gboolean
command_server_validate_socket_directory(CommandServer *server, GError **error)
{
  g_autofree char *directory = g_path_get_dirname(server->path);
  struct stat directory_stat;
  mode_t permissions;

  if (lstat(directory, &directory_stat) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to inspect command socket directory %s: %s. Set XDG_RUNTIME_DIR to a writable private directory.",
                directory,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (S_ISLNK(directory_stat.st_mode)) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "refusing command socket directory %s because it is a symlink. Remove it or set XDG_RUNTIME_DIR to a private runtime directory.",
                directory);
    return FALSE;
  }
  if (!S_ISDIR(directory_stat.st_mode)) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "command socket parent %s is a %s. Remove the conflicting path or set XDG_RUNTIME_DIR to a writable private directory.",
                directory,
                command_socket_file_type_name(directory_stat.st_mode));
    return FALSE;
  }
  if (directory_stat.st_uid != getuid()) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "refusing command socket directory %s because it is not owned by this user. Fix ownership or set XDG_RUNTIME_DIR to a private runtime directory.",
                directory);
    return FALSE;
  }

  permissions = directory_stat.st_mode & 0777;
  if ((permissions & 0077) != 0) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "refusing command socket directory %s with permissions %03o. Run 'chmod 700 %s' or set XDG_RUNTIME_DIR to a private runtime directory.",
                directory,
                permissions,
                directory);
    return FALSE;
  }

  return TRUE;
}

static gboolean
command_server_wait_for_startup_lock(int fd, const char *socket_path, GError **error)
{
  gint64 deadline = g_get_monotonic_time() +
                   (gint64)(COMMAND_SERVER_STARTUP_LOCK_TIMEOUT_SECONDS * G_USEC_PER_SEC);

  while (TRUE) {
    if (flock(fd, LOCK_EX | LOCK_NB) == 0)
      return TRUE;
    if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EACCES) {
      int saved_errno = errno;
      g_set_error(error,
                  G_IO_ERROR,
                  g_io_error_from_errno(saved_errno),
                  "Unable to lock command socket startup lock: %s",
                  g_strerror(saved_errno));
      return FALSE;
    }
    if (g_get_monotonic_time() >= deadline) {
      g_set_error(error,
                  G_IO_ERROR,
                  G_IO_ERROR_BUSY,
                  "another %s process is still preparing command socket %s. Retry after it finishes, or stop the stuck %s process.",
                  COMMAND_NAME,
                  socket_path,
                  COMMAND_NAME);
      return FALSE;
    }
    g_usleep((gulong)(COMMAND_SERVER_STARTUP_LOCK_RETRY_SECONDS * G_USEC_PER_SEC));
  }
}

static gboolean
command_server_acquire_startup_lock(CommandServer *server, GError **error)
{
  int flags = O_RDWR | O_CREAT | O_CLOEXEC;
  struct stat lock_stat;

#ifdef O_NOFOLLOW
  flags |= O_NOFOLLOW;
#endif

  server->startup_lock_fd = open(server->startup_lock_path, flags, 0600);
  if (server->startup_lock_fd < 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to open command socket startup lock %s: %s. Remove conflicting paths or check runtime directory permissions.",
                server->startup_lock_path,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (fstat(server->startup_lock_fd, &lock_stat) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to inspect command socket startup lock %s: %s",
                server->startup_lock_path,
                g_strerror(saved_errno));
    return FALSE;
  }
  if (!S_ISREG(lock_stat.st_mode)) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "command socket startup lock %s is a %s. Remove it or set XDG_RUNTIME_DIR to a different private runtime directory.",
                server->startup_lock_path,
                command_socket_file_type_name(lock_stat.st_mode));
    return FALSE;
  }
  if (lock_stat.st_uid != getuid()) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "refusing command socket startup lock %s because it is not owned by this user. Fix ownership or set XDG_RUNTIME_DIR to a private runtime directory.",
                server->startup_lock_path);
    return FALSE;
  }

  return command_server_wait_for_startup_lock(server->startup_lock_fd, server->path, error);
}

static gboolean
command_server_socket_path_is_unchanged(const char *path, const struct stat *expected, gboolean *disappeared, GError **error)
{
  struct stat current;

  if (disappeared)
    *disappeared = FALSE;

  if (lstat(path, &current) != 0) {
    if (errno == ENOENT) {
      if (disappeared)
        *disappeared = TRUE;
      return FALSE;
    }
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to inspect command socket path %s: %s. Remove the path manually or set XDG_RUNTIME_DIR to a writable private directory.",
                path,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (!S_ISSOCK(current.st_mode) || current.st_dev != expected->st_dev || current.st_ino != expected->st_ino) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "command socket path %s changed while startup was preparing it. Try again or remove the conflicting path manually.",
                path);
    return FALSE;
  }

  return TRUE;
}

static gboolean
command_server_remove_stale_socket_path(CommandServer *server, GError **error)
{
  struct stat existing;
  CommandSocketProbeStatus probe_status;
  gboolean disappeared = FALSE;

  if (lstat(server->path, &existing) != 0) {
    if (errno == ENOENT)
      return TRUE;
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to inspect command socket path %s: %s. Remove the path or set XDG_RUNTIME_DIR to a writable private directory.",
                server->path,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (!S_ISSOCK(existing.st_mode)) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "%s is a %s and blocks the command socket. Remove it or set XDG_RUNTIME_DIR to a different private runtime directory.",
                server->path,
                command_socket_file_type_name(existing.st_mode));
    return FALSE;
  }

  probe_status = command_socket_probe(server->path);
  if (probe_status == COMMAND_SOCKET_PROBE_ACKNOWLEDGED) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_EXISTS,
                "another %s process is already listening on command socket %s. Use --quit to stop the running instance or remove the socket only if that process is gone.",
                COMMAND_NAME,
                server->path);
    return FALSE;
  }
  if (probe_status == COMMAND_SOCKET_PROBE_UNACKNOWLEDGED) {
    g_set_error(error,
                G_IO_ERROR,
                G_IO_ERROR_BUSY,
                "command socket %s exists but did not acknowledge a ping. A running instance may be busy or a foreign listener may be using the path; retry, or remove the socket only if that process is gone.",
                server->path);
    return FALSE;
  }

  if (!command_server_socket_path_is_unchanged(server->path, &existing, &disappeared, error))
    return disappeared;

  if (unlink(server->path) != 0 && errno != ENOENT) {
    int saved_errno = errno;
    g_set_error(error,
                G_IO_ERROR,
                g_io_error_from_errno(saved_errno),
                "Unable to remove stale command socket %s: %s. Remove it only if no %s process is running, or set XDG_RUNTIME_DIR to a different private runtime directory.",
                server->path,
                g_strerror(saved_errno),
                COMMAND_NAME);
    return FALSE;
  }

  return TRUE;
}

static gboolean
command_server_bind_socket(CommandServer *server, GError **error)
{
  g_autoptr(GSocketAddress) address = NULL;
  struct stat bound_stat;

  if (!command_server_validate_socket_directory(server, error))
    return FALSE;
  if (!command_server_acquire_startup_lock(server, error))
    return FALSE;
  if (!command_server_remove_stale_socket_path(server, error))
    return FALSE;

  address = g_unix_socket_address_new(server->path);
  if (!g_socket_bind(server->socket, address, FALSE, error))
    return FALSE;

  if (lstat(server->path, &bound_stat) == 0) {
    server->bound_dev = bound_stat.st_dev;
    server->bound_ino = bound_stat.st_ino;
    server->has_bound_socket_id = TRUE;
  }

  if (g_chmod(server->path, 0600) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_FILE_ERROR,
                g_file_error_from_errno(saved_errno),
                "Unable to chmod %s: %s. Check ownership and permissions on the runtime directory.",
                server->path,
                g_strerror(saved_errno));
    return FALSE;
  }

  if (!g_socket_listen(server->socket, error))
    return FALSE;

  g_socket_set_timeout(server->socket, COMMAND_SOCKET_IO_TIMEOUT_SECONDS);
  command_server_release_startup_lock(server);
  return TRUE;
}

static gboolean
command_server_path_matches_bound_socket(CommandServer *server)
{
  struct stat current;

  if (!server->has_bound_socket_id)
    return FALSE;
  if (lstat(server->path, &current) != 0)
    return FALSE;
  if (!S_ISSOCK(current.st_mode))
    return FALSE;
  return current.st_dev == server->bound_dev && current.st_ino == server->bound_ino;
}

gboolean
command_server_is_healthy(CommandServer *server)
{
  if (!server || g_atomic_int_get(&server->closed))
    return FALSE;
  if (!command_server_path_matches_bound_socket(server))
    return FALSE;
  if (!command_socket_acknowledges_ping(server->path))
    return FALSE;
  return command_server_path_matches_bound_socket(server);
}

static CommandServer *
command_server_ref(CommandServer *server)
{
  g_atomic_int_inc(&server->ref_count);
  return server;
}

static void
command_server_unref(CommandServer *server)
{
  if (!server || !g_atomic_int_dec_and_test(&server->ref_count))
    return;

  g_clear_object(&server->socket);
  if (server->destroy && server->user_data)
    server->destroy(server->user_data);
  g_free(server->path);
  g_free(server->startup_lock_path);
  g_free(server);
}

static void
command_server_release_startup_lock(CommandServer *server)
{
  if (!server || server->startup_lock_fd < 0)
    return;
  flock(server->startup_lock_fd, LOCK_UN);
  close(server->startup_lock_fd);
  server->startup_lock_fd = -1;
}

static void
command_server_unlink_bound_socket_path(CommandServer *server)
{
  gboolean acquired_lock = FALSE;

  if (!server || !server->has_bound_socket_id)
    return;
  if (server->startup_lock_fd < 0) {
    if (!command_server_acquire_startup_lock(server, NULL))
      return;
    acquired_lock = TRUE;
  }
  if (command_server_path_matches_bound_socket(server))
    unlink(server->path);
  if (acquired_lock)
    command_server_release_startup_lock(server);
}

void
command_server_close(CommandServer *server)
{
  if (!server || !g_atomic_int_compare_and_exchange(&server->closed, FALSE, TRUE))
    return;

  if (server->socket)
    g_socket_close(server->socket, NULL);

  if (server->thread) {
    g_thread_join(server->thread);
    server->thread = NULL;
  }

  command_server_unlink_bound_socket_path(server);
  command_server_release_startup_lock(server);
}

void
command_server_free(CommandServer *server)
{
  if (!server)
    return;

  command_server_close(server);
  command_server_unref(server);
}
