#include "command/command-ipc.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#define COMMAND_IPC_TIMEOUT_SECONDS 1

static void
command_ipc_set_socket_timeout(int fd, double seconds)
{
  struct timeval timeout;

  timeout.tv_sec = (time_t)seconds;
  timeout.tv_usec = (suseconds_t)((seconds - timeout.tv_sec) * 1000000.0);
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

static gboolean
command_ipc_send_all(int fd, const char *payload)
{
  gsize offset = 0;
  gsize total = strlen(payload);

  while (offset < total) {
    ssize_t sent = send(fd, payload + offset, total - offset, MSG_NOSIGNAL);

    if (sent < 0 && errno == EINTR)
      continue;
    if (sent <= 0)
      return FALSE;
    offset += (gsize)sent;
  }

  return TRUE;
}

static CommandIpcStatus
command_ipc_read_response(int fd, SidebarCommand command)
{
  char response[IPC_MAX_RESPONSE_BYTES + 1] = { 0 };
  gsize used = 0;

  while (used < IPC_MAX_RESPONSE_BYTES) {
    ssize_t received = recv(fd, response + used, IPC_MAX_RESPONSE_BYTES - used, 0);

    if (received < 0 && errno == EINTR)
      continue;
    if (received <= 0)
      return COMMAND_IPC_DELIVERED;
    if (memchr(response + used, '\n', (gsize)received)) {
      used += (gsize)received;
      response[MIN(used, IPC_MAX_RESPONSE_BYTES)] = '\0';
      return command_ipc_response_is_success(response, command) ? COMMAND_IPC_ACKNOWLEDGED
                                                                : COMMAND_IPC_DELIVERED;
    }
    used += (gsize)received;
  }

  return COMMAND_IPC_DELIVERED;
}

CommandIpcStatus
command_ipc_send(SidebarCommand command, const char *target_output_name, GError **error)
{
  g_autofree char *path = command_ipc_dup_socket_path(error);
  g_autofree char *payload = NULL;
  struct sockaddr_un address;
  int fd = -1;
  CommandIpcStatus status = COMMAND_IPC_NOT_DELIVERED;

  if (!path || !sidebar_command_is_socket_command(command))
    return COMMAND_IPC_NOT_DELIVERED;

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    return COMMAND_IPC_NOT_DELIVERED;

  command_ipc_set_socket_timeout(fd, COMMAND_IPC_TIMEOUT_SECONDS);

  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  if (strlen(path) >= sizeof(address.sun_path))
    goto out;
  g_strlcpy(address.sun_path, path, sizeof(address.sun_path));

  if (connect(fd, (struct sockaddr *)&address, sizeof(address)) != 0)
    goto out;

  payload = command_ipc_encode(command, target_output_name);
  if (!command_ipc_send_all(fd, payload))
    goto out;

  status = command_ipc_read_response(fd, command);

out:
  close(fd);
  return status;
}
