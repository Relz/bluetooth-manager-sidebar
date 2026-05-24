#include "obex/obex-service.h"

#include "obex/obex-service-components.h"
#include "obex/obex-service-private.h"

struct _ObexService {
  guint ref_count;
  gboolean disposed;
  ObexServiceComponents *components;
};

ObexService *
obex_service_ref(ObexService *service)
{
  service->ref_count++;
  return service;
}

void
obex_service_unref(ObexService *service)
{
  service->ref_count--;
  if (service->ref_count != 0)
    return;

  g_clear_pointer(&service->components, obex_service_components_free);
  g_free(service);
}

gboolean
obex_service_is_disposed(ObexService *service)
{
  return !service || service->disposed;
}

void
obex_service_mark_disposed(ObexService *service)
{
  if (service)
    service->disposed = TRUE;
}

ObexServiceComponents *
obex_service_components(ObexService *service)
{
  return service ? service->components : NULL;
}

ObexService *
obex_service_new(const ObexServiceDependencies *dependencies, gpointer user_data, GDestroyNotify destroy)
{
  ObexService *service = g_new0(ObexService, 1);

  service->ref_count = 1;
  service->components = obex_service_components_new(dependencies, user_data, destroy);
  return service;
}

void
obex_service_free(ObexService *service)
{
  if (!service)
    return;

  obex_service_shutdown(service);
  obex_service_mark_disposed(service);
  obex_service_context_clear_dependencies(obex_service_components_context(service->components));
  obex_service_unref(service);
}
