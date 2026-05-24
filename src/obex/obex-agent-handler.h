#pragma once

#include "obex/obex-service.h"

gboolean obex_agent_handler_register_object(ObexService *service, GError **error);
gboolean obex_agent_handler_register_with_obex(ObexService *service, GError **error);
void obex_agent_handler_unregister_from_obex(ObexService *service);
void obex_agent_handler_unregister_object(ObexService *service);
