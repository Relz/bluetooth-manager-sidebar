#pragma once

#include "obex/obex-service.h"

void obex_incoming_reject_active(ObexService *service,
                                 const char *error_name,
                                 const char *message);
void obex_incoming_accept_active(ObexService *service);
void obex_incoming_authorize_push(ObexService *service,
                                  const char *transfer_path,
                                  GDBusMethodInvocation *invocation);
gboolean obex_incoming_handle_transfer_status(ObexService *service,
                                              const char *transfer_path,
                                              const char *status);
void obex_incoming_cleanup_transfer_foreach(gpointer key, gpointer value, gpointer user_data);
