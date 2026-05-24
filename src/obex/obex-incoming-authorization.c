#include "obex/obex-incoming-authorization.h"

#include "obex/obex-service-policy-private.h"

gboolean
obex_incoming_authorization_should_auto_accept(ObexService *service,
                                               const ObexTransferDetails *details)
{
  return details && obex_service_auto_accept_trusted(service, details->destination);
}
