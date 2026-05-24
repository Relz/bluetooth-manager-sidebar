#pragma once

#include <gio/gio.h>

const char *bluez_pairing_agent_contract_object_path(void);
const char *bluez_pairing_agent_contract_capability(void);
GDBusNodeInfo *bluez_pairing_agent_contract_node_info_new(GError **error);
