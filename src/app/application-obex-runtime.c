#include "app/application-obex-runtime.h"

#include "app/application-obex-adapter.h"

struct _ApplicationObexRuntime {
  ObexService *service;
};

ApplicationObexRuntime *
application_obex_runtime_new(Application *self)
{
  ApplicationObexRuntime *runtime = g_new0(ApplicationObexRuntime, 1);
  ObexServiceDependencies dependencies = { 0 };

  application_obex_adapter_init_dependencies(self, &dependencies);
  runtime->service = obex_service_new(&dependencies, self, NULL);
  return runtime;
}

gboolean
application_obex_runtime_start(ApplicationObexRuntime *runtime)
{
  return runtime && runtime->service && obex_service_start(runtime->service);
}

void
application_obex_runtime_free(ApplicationObexRuntime *runtime)
{
  if (!runtime)
    return;

  g_clear_pointer(&runtime->service, obex_service_free);
  g_free(runtime);
}

ObexService *
application_obex_runtime_service(ApplicationObexRuntime *runtime)
{
  return runtime ? runtime->service : NULL;
}
