#include "obex/obex-incoming-controller.h"

#include "obex/obex-incoming-active-authorization.h"
#include "obex/obex-incoming-authorization.h"
#include "obex/obex-incoming-authorization-acceptance.h"
#include "obex/obex-service-agent-private.h"

void
obex_incoming_authorize_push(ObexService *service,
                             const char *transfer_path,
                             GDBusMethodInvocation *invocation)
{
  ObexTransferDetails *details = obex_transfer_details_read(obex_service_connection(service),
                                                               transfer_path);

  if (obex_incoming_authorization_should_auto_accept(service, details)) {
    obex_incoming_authorization_accept_transfer(service, details, invocation);
    obex_transfer_details_free(details);
    return;
  }

  obex_incoming_authorization_store_pending(service, invocation, details);
}
