#pragma once

#include <gio/gio.h>

typedef struct _ObexAgentLifecycle ObexAgentLifecycle;

ObexAgentLifecycle *obex_agent_lifecycle_new(void);
void obex_agent_lifecycle_free(ObexAgentLifecycle *lifecycle);

GDBusConnection *obex_agent_lifecycle_connection(ObexAgentLifecycle *lifecycle);
void obex_agent_lifecycle_take_connection(ObexAgentLifecycle *lifecycle,
                                          GDBusConnection *connection);
void obex_agent_lifecycle_clear_connection(ObexAgentLifecycle *lifecycle);

GDBusNodeInfo *obex_agent_lifecycle_node_info(ObexAgentLifecycle *lifecycle);
void obex_agent_lifecycle_take_node_info(ObexAgentLifecycle *lifecycle,
                                         GDBusNodeInfo *node_info);
void obex_agent_lifecycle_clear_node_info(ObexAgentLifecycle *lifecycle);

guint obex_agent_lifecycle_registration_id(ObexAgentLifecycle *lifecycle);
void obex_agent_lifecycle_set_registration_id(ObexAgentLifecycle *lifecycle,
                                               guint registration_id);

gboolean obex_agent_lifecycle_registered_with_obex(ObexAgentLifecycle *lifecycle);
void obex_agent_lifecycle_set_registered_with_obex(ObexAgentLifecycle *lifecycle,
                                                   gboolean registered);
