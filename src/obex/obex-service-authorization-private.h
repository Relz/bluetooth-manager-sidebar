#pragma once

#include "obex/obex-service.h"
#include "obex/obex-pending-authorization.h"

ObexPendingAuthorization *obex_service_active_authorization(ObexService *service);
ObexPendingAuthorization *obex_service_steal_active_authorization(ObexService *service);
void obex_service_set_active_authorization(ObexService *service,
                                           ObexPendingAuthorization *request);
