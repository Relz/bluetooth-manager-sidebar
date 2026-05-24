#pragma once

#include "obex/obex-service.h"

typedef struct _ObexServiceComponents ObexServiceComponents;

ObexService *obex_service_ref(ObexService *service);
void obex_service_unref(ObexService *service);
gboolean obex_service_is_disposed(ObexService *service);
void obex_service_mark_disposed(ObexService *service);
ObexServiceComponents *obex_service_components(ObexService *service);
