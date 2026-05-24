#include "obex/obex-incoming-active-authorization.h"

#include "obex/obex-dbus.h"
#include "obex/obex-incoming-controller.h"
#include "obex/obex-incoming-notifications.h"
#include "obex/obex-service-authorization-private.h"

void
obex_incoming_authorization_store_pending(ObexService *service,
                                          GDBusMethodInvocation *invocation,
                                          ObexTransferDetails *details)
{
  obex_incoming_reject_active(service,
                              OBEX_ERROR_REJECTED,
                              "Another file transfer request replaced this one");
  obex_service_set_active_authorization(service,
                                        obex_pending_authorization_new(invocation, details));
  obex_incoming_notifications_send_authorization(service, details);
}
