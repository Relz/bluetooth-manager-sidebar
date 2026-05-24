#include "command/command-socket.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#include "command/command-codec.h"

static void
command_socket_set_fd_timeout(int fd, double seconds)
{
  struct timeval timeout;

  timeout.tv_sec = (time_t)seconds;
  timeout.tv_usec = (suseconds_t)((seconds - timeout.tv_sec) * 1000000.0);
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

static gboolean
command_socket_send_fd_all(int fd, const char *data, gsize length)
{
  while (length > 0) {
    ssize_t written = send(fd, data, length, MSG_NOSIGNAL);

    if (written < 0 && errno == EINTR)
      continue;
    if (written <= 0)
      return FALSE;
    data += written;
    length -= (gsize)written;
  }

  return TRUE;
}

CommandSocketProbeStatus
command_socket_probe(const char *path)
{
  g_autofree char *expected = command_ipc_success_response(SIDEBAR_COMMAND_PING);
  char response[IPC_MAX_RESPONSE_BYTES + 1] = { 0 };
  gsize used = 0;
  struct sockaddr_un address;
  int fd = -1;
  CommandSocketProbeStatus status = COMMAND_SOCKET_PROBE_UNACKNOWLEDGED;

  if (!path || !*path)
    return COMMAND_SOCKET_PROBE_NOT_CONNECTABLE;

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    return COMMAND_SOCKET_PROBE_UNACKNOWLEDGED;

  command_socket_set_fd_timeout(fd, COMMAND_SOCKET_PROBE_TIMEOUT_SECONDS);

  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  if (strlen(path) >= sizeof(address.sun_path))
    goto out;
  g_strlcpy(address.sun_path, path, sizeof(address.sun_path));

  if (connect(fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
    if (errno == ECONNREFUSED || errno == ENOENT)
      status = COMMAND_SOCKET_PROBE_NOT_CONNECTABLE;
    goto out;
  }

  if (!command_socket_send_fd_all(fd, "ping\n", 5))
    goto out;

  while (used < IPC_MAX_RESPONSE_BYTES) {
    ssize_t received = recv(fd, response + used, IPC_MAX_RESPONSE_BYTES - used, 0);

    if (received < 0) {
      if (errno == EINTR)
        continue;
      goto out;
    }
    if (received == 0)
      goto out;
    if (memchr(response + used, '\n', (gsize)received)) {
      used += (gsize)received;
      break;
    }
    used += (gsize)received;
  }

  response[MIN(used, IPC_MAX_RESPONSE_BYTES)] = '\0';
  if (g_strcmp0(response, expected) == 0)
    status = COMMAND_SOCKET_PROBE_ACKNOWLEDGED;

out:
  close(fd);
  return status;
}

gboolean
command_socket_acknowledges_ping(const char *path)
{
  return command_socket_probe(path) == COMMAND_SOCKET_PROBE_ACKNOWLEDGED;
}

gboolean
command_socket_send_all(GSocket *socket, const char *payload)
{
  gsize offset = 0;
  gsize total = strlen(payload);

  while (offset < total) {
    gssize sent = g_socket_send(socket, payload + offset, total - offset, NULL, NULL);

    if (sent <= 0)
      return FALSE;
    offset += (gsize)sent;
  }

  return TRUE;
}

void
command_socket_send_success(GSocket *socket, SidebarCommand command)
{
  g_autofree char *response = command_ipc_success_response(command);

  command_socket_send_all(socket, response);
}

gboolean
command_socket_read_command(GSocket *connection,
                            SidebarCommand *out_command,
                            char **out_target_output_name)
{
  char data[IPC_MAX_COMMAND_BYTES + 1] = { 0 };
  gsize used = 0;
  char *newline = NULL;

  g_return_val_if_fail(out_command != NULL, FALSE);
  g_return_val_if_fail(out_target_output_name != NULL, FALSE);

  *out_target_output_name = NULL;
  g_socket_set_timeout(connection, COMMAND_SOCKET_IO_TIMEOUT_SECONDS);

  while (used < IPC_MAX_COMMAND_BYTES) {
    gssize received = g_socket_receive(connection,
                                       data + used,
                                       IPC_MAX_COMMAND_BYTES - used,
                                       NULL,
                                       NULL);

    if (received <= 0)
      return FALSE;
    newline = memchr(data + used, '\n', (gsize)received);
    used += (gsize)received;
    if (newline)
      break;
  }

  if (!newline)
    return FALSE;
  *newline = '\0';

  return command_ipc_decode_line(data, out_command, out_target_output_name);
}

const char *
command_socket_file_type_name(mode_t mode)
{
  if (S_ISDIR(mode))
    return "directory";
  if (S_ISLNK(mode))
    return "symlink";
  if (S_ISREG(mode))
    return "regular file";
  if (S_ISSOCK(mode))
    return "socket";
  if (S_ISFIFO(mode))
    return "FIFO";
  if (S_ISBLK(mode))
    return "block device";
  if (S_ISCHR(mode))
    return "character device";
  return "filesystem object";
}
