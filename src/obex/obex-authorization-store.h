#pragma once

#include "obex/obex-pending-authorization.h"

typedef struct _ObexAuthorizationStore ObexAuthorizationStore;

ObexAuthorizationStore *obex_authorization_store_new(void);
void obex_authorization_store_free(ObexAuthorizationStore *store);
ObexPendingAuthorization *obex_authorization_store_active(ObexAuthorizationStore *store);
ObexPendingAuthorization *obex_authorization_store_steal_active(ObexAuthorizationStore *store);
void obex_authorization_store_set_active(ObexAuthorizationStore *store,
                                         ObexPendingAuthorization *request);
