#pragma once

#include <gio/gio.h>

#include "obex/obex-service-callbacks.h"

typedef struct _ObexService ObexService;

ObexService *obex_service_new(const ObexServiceDependencies *dependencies,
                                      gpointer user_data,
                                      GDestroyNotify destroy);
void obex_service_free(ObexService *service);

gboolean obex_service_start(ObexService *service);
void obex_service_shutdown(ObexService *service);
