#pragma once

#include "app/application.h"
#include "obex/obex-service.h"

typedef struct _ApplicationObexRuntime ApplicationObexRuntime;

ApplicationObexRuntime *application_obex_runtime_new(Application *self);
gboolean application_obex_runtime_start(ApplicationObexRuntime *runtime);
void application_obex_runtime_free(ApplicationObexRuntime *runtime);

ObexService *application_obex_runtime_service(ApplicationObexRuntime *runtime);
