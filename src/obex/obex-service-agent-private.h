#pragma once

#include "obex/obex-service.h"

GDBusConnection *obex_service_connection(ObexService *service);
void obex_service_agent_take_connection(ObexService *service, GDBusConnection *connection);
GDBusNodeInfo *obex_service_agent_node_info(ObexService *service);
void obex_service_agent_take_node_info(ObexService *service, GDBusNodeInfo *node_info);
void obex_service_agent_clear_node_info(ObexService *service);
void obex_service_agent_clear_connection(ObexService *service);
guint obex_service_agent_registration_id(ObexService *service);
void obex_service_agent_set_registration_id(ObexService *service, guint registration_id);
gboolean obex_service_agent_registered_with_obex(ObexService *service);
void obex_service_agent_set_registered_with_obex(ObexService *service, gboolean registered);
