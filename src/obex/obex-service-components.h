#pragma once

#include "obex/obex-authorization-store.h"
#include "obex/obex-agent-lifecycle.h"
#include "obex/obex-service-context.h"
#include "obex/obex-transfer-monitor.h"
#include "obex/obex-transfer-registry.h"

typedef struct _ObexServiceComponents ObexServiceComponents;

ObexServiceComponents *obex_service_components_new(const ObexServiceDependencies *dependencies,
                                                       gpointer user_data,
                                                       GDestroyNotify destroy);
void obex_service_components_free(ObexServiceComponents *components);

ObexAgentLifecycle *obex_service_components_agent(ObexServiceComponents *components);
ObexAuthorizationStore *obex_service_components_authorizations(ObexServiceComponents *components);
ObexTransferRegistry *obex_service_components_transfers(ObexServiceComponents *components);
ObexServiceContext *obex_service_components_context(ObexServiceComponents *components);
void obex_service_components_take_transfer_monitor(ObexServiceComponents *components,
                                                   ObexTransferMonitor *monitor);
void obex_service_components_clear_transfer_monitor(ObexServiceComponents *components);
