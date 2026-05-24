#pragma once

#include "obex/obex-service.h"

typedef struct _ObexTransferMonitor ObexTransferMonitor;

void obex_service_take_transfer_monitor(ObexService *service,
                                        ObexTransferMonitor *monitor);
void obex_service_clear_transfer_monitor(ObexService *service);
