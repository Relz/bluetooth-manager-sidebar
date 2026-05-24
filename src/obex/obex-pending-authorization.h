#pragma once

#include <gio/gio.h>

#include "obex/obex-transfer-details.h"

typedef struct _ObexPendingAuthorization ObexPendingAuthorization;

ObexPendingAuthorization *obex_pending_authorization_new(GDBusMethodInvocation *invocation,
                                                            ObexTransferDetails *details);
void obex_pending_authorization_free(ObexPendingAuthorization *request);
GDBusMethodInvocation *obex_pending_authorization_invocation(ObexPendingAuthorization *request);
ObexTransferDetails *obex_pending_authorization_details(ObexPendingAuthorization *request);
const char *obex_pending_authorization_path(ObexPendingAuthorization *request);
