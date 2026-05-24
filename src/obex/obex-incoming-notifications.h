#pragma once

#include "obex/obex-receiver.h"
#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

void obex_incoming_notifications_withdraw(ObexService *service);
void obex_incoming_notifications_send_authorization(ObexService *service,
                                                    ObexTransferDetails *details);
char *obex_incoming_notifications_notify_location_error(ObexService *service,
                                                        const char *detail);
void obex_incoming_notifications_notify_receiving(ObexService *service,
                                                  ObexIncomingTransfer *transfer);
void obex_incoming_notifications_notify_save_error(ObexService *service,
                                                   const char *detail);
void obex_incoming_notifications_notify_saved(ObexService *service,
                                              const char *final_path);
void obex_incoming_notifications_notify_failed(ObexService *service);
