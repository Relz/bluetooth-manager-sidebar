#pragma once

#include "obex/obex-service.h"
#include "obex/obex-transfer-monitor.h"

gboolean obex_service_lifecycle_connect_session_bus(ObexService *service, GError **error);
gboolean obex_service_lifecycle_register_agent_object(ObexService *service, GError **error);
void obex_service_lifecycle_start_transfer_monitor(ObexService *service,
                                                   ObexTransferStatusHandler handler,
                                                   gpointer user_data);
gboolean obex_service_lifecycle_register_agent_with_obex(ObexService *service, GError **error);

void obex_service_lifecycle_cancel_incoming(ObexService *service);
void obex_service_lifecycle_withdraw_notifications(ObexService *service);
void obex_service_lifecycle_unregister_agent(ObexService *service);
void obex_service_lifecycle_stop_transfer_monitor(ObexService *service);
void obex_service_lifecycle_clear_transfers(ObexService *service);
void obex_service_lifecycle_clear_agent_state(ObexService *service);
