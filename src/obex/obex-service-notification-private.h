#pragma once

#include "obex/obex-service.h"

void obex_service_notify(ObexService *service, const char *message);
void obex_service_set_status(ObexService *service, const char *message);
void obex_service_send_notification(ObexService *service,
                                    const char *id,
                                    GNotification *notification);
void obex_service_withdraw_notification(ObexService *service, const char *id);
void obex_service_send_completed_notification(ObexService *service, const char *final_path);
