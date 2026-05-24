#pragma once

#include "obex/obex-service-callbacks.h"

typedef struct _ObexServiceContext ObexServiceContext;

ObexServiceContext *obex_service_context_new(const ObexServiceDependencies *dependencies,
                                                 gpointer user_data,
                                                 GDestroyNotify destroy);
void obex_service_context_free(ObexServiceContext *context);

const ObexPolicyProvider *obex_service_context_policy_provider(ObexServiceContext *context);
const ObexDeviceResolver *obex_service_context_device_resolver(ObexServiceContext *context);
const ObexStatusSink *obex_service_context_status_sink(ObexServiceContext *context);
const ObexNotificationSink *obex_service_context_notification_sink(ObexServiceContext *context);
gpointer obex_service_context_user_data(ObexServiceContext *context);
char **obex_service_context_status_message_slot(ObexServiceContext *context);
const char *obex_service_context_status_message_value(ObexServiceContext *context);
void obex_service_context_clear_dependencies(ObexServiceContext *context);
