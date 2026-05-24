#pragma once

#include "obex/obex-service.h"

void obex_service_remove_session_on_connection(GDBusConnection *connection, const char *session_path);
void obex_service_remove_session(ObexService *service, const char *session_path);
