#pragma once

#include <gio/gio.h>

const char *obex_agent_path(void);
GDBusNodeInfo *obex_agent_node_info_new(GError **error);
