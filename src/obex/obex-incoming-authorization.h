#pragma once

#include "obex/obex-service.h"
#include "obex/obex-transfer-details.h"

gboolean obex_incoming_authorization_should_auto_accept(ObexService *service,
                                                       const ObexTransferDetails *details);
