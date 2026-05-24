#pragma once

#include <glib.h>

char *command_ipc_dup_socket_path(GError **error);
void command_ipc_unlink_socket_path(const char *path);
