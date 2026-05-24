#include "obex/obex-service-authorization-private.h"

#include "obex/obex-authorization-store.h"
#include "obex/obex-service-components.h"
#include "obex/obex-service-private.h"

struct _ObexAuthorizationStore {
  ObexPendingAuthorization *active_request;
};

struct _ObexPendingAuthorization {
  GDBusMethodInvocation *invocation;
  ObexTransferDetails *details;
};

ObexAuthorizationStore *
obex_authorization_store_new(void)
{
  return g_new0(ObexAuthorizationStore, 1);
}

void
obex_authorization_store_free(ObexAuthorizationStore *store)
{
  if (!store)
    return;

  g_clear_pointer(&store->active_request, obex_pending_authorization_free);
  g_free(store);
}

ObexPendingAuthorization *
obex_authorization_store_active(ObexAuthorizationStore *store)
{
  return store ? store->active_request : NULL;
}

ObexPendingAuthorization *
obex_authorization_store_steal_active(ObexAuthorizationStore *store)
{
  return store ? g_steal_pointer(&store->active_request) : NULL;
}

void
obex_authorization_store_set_active(ObexAuthorizationStore *store,
                                    ObexPendingAuthorization *request)
{
  if (!store)
    return;

  g_clear_pointer(&store->active_request, obex_pending_authorization_free);
  store->active_request = request;
}

ObexPendingAuthorization *
obex_pending_authorization_new(GDBusMethodInvocation *invocation,
                               ObexTransferDetails *details)
{
  ObexPendingAuthorization *request = g_new0(ObexPendingAuthorization, 1);

  request->invocation = g_object_ref(invocation);
  request->details = details;
  return request;
}

void
obex_pending_authorization_free(ObexPendingAuthorization *request)
{
  if (!request)
    return;

  g_clear_object(&request->invocation);
  obex_transfer_details_free(request->details);
  g_free(request);
}

GDBusMethodInvocation *
obex_pending_authorization_invocation(ObexPendingAuthorization *request)
{
  return request ? request->invocation : NULL;
}

ObexTransferDetails *
obex_pending_authorization_details(ObexPendingAuthorization *request)
{
  return request ? request->details : NULL;
}

const char *
obex_pending_authorization_path(ObexPendingAuthorization *request)
{
  return (request && request->details) ? request->details->path : NULL;
}

ObexPendingAuthorization *
obex_service_active_authorization(ObexService *service)
{
  return obex_authorization_store_active(
    obex_service_components_authorizations(obex_service_components(service)));
}

ObexPendingAuthorization *
obex_service_steal_active_authorization(ObexService *service)
{
  return obex_authorization_store_steal_active(
    obex_service_components_authorizations(obex_service_components(service)));
}

void
obex_service_set_active_authorization(ObexService *service,
                                      ObexPendingAuthorization *request)
{
  obex_authorization_store_set_active(
    obex_service_components_authorizations(obex_service_components(service)),
    request);
}
