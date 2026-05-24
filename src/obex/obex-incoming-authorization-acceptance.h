#pragma once

#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

gboolean obex_incoming_authorization_accept_transfer(ObexService *service,
                                                     ObexTransferDetails *details,
                                                     GDBusMethodInvocation *invocation);
