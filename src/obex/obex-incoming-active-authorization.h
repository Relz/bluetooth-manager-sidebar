#pragma once

#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

void obex_incoming_authorization_store_pending(ObexService *service,
                                               GDBusMethodInvocation *invocation,
                                               ObexTransferDetails *details);
