#pragma once

#include "obex/obex-service.h"

void obex_outgoing_notifications_notify_preparing(ObexService *service);
void obex_outgoing_notifications_notify_prepare_failed(ObexService *service, const char *detail);
void obex_outgoing_notifications_notify_prepare_bus_unavailable(ObexService *service);
void obex_outgoing_notifications_notify_sending(ObexService *service);
void obex_outgoing_notifications_notify_send_failed(ObexService *service, const char *detail);
void obex_outgoing_notifications_notify_send_bus_unavailable(ObexService *service);
void obex_outgoing_notifications_notify_sending_file(ObexService *service, const char *file_name);
void obex_outgoing_notifications_notify_sent(ObexService *service, const char *file_name);
void obex_outgoing_notifications_notify_failed(ObexService *service);
void obex_outgoing_notifications_notify_rejected(ObexService *service,
                                                 const char *device_name,
                                                 const char *file_name);
void obex_outgoing_notifications_notify_device_address_unavailable(ObexService *service);
void obex_outgoing_notifications_notify_file_validation_error(ObexService *service, const char *message);
