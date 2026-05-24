#include "obex/obex-service-context.h"

struct _ObexServiceContext {
  char *status_message;
  ObexServiceDependencies dependencies;
  gpointer user_data;
  GDestroyNotify destroy;
};

ObexServiceContext *
obex_service_context_new(const ObexServiceDependencies *dependencies,
                         gpointer user_data,
                         GDestroyNotify destroy)
{
  ObexServiceContext *context = g_new0(ObexServiceContext, 1);

  if (dependencies)
    context->dependencies = *dependencies;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

void
obex_service_context_free(ObexServiceContext *context)
{
  if (!context)
    return;

  g_free(context->status_message);
  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context);
}

const ObexPolicyProvider *
obex_service_context_policy_provider(ObexServiceContext *context)
{
  return context ? &context->dependencies.policy : NULL;
}

const ObexDeviceResolver *
obex_service_context_device_resolver(ObexServiceContext *context)
{
  return context ? &context->dependencies.device_resolver : NULL;
}

const ObexStatusSink *
obex_service_context_status_sink(ObexServiceContext *context)
{
  return context ? &context->dependencies.status : NULL;
}

const ObexNotificationSink *
obex_service_context_notification_sink(ObexServiceContext *context)
{
  return context ? &context->dependencies.notifications : NULL;
}

gpointer
obex_service_context_user_data(ObexServiceContext *context)
{
  return context ? context->user_data : NULL;
}

char **
obex_service_context_status_message_slot(ObexServiceContext *context)
{
  return context ? &context->status_message : NULL;
}

const char *
obex_service_context_status_message_value(ObexServiceContext *context)
{
  return context ? context->status_message : NULL;
}

void
obex_service_context_clear_dependencies(ObexServiceContext *context)
{
  if (context)
    context->dependencies = (ObexServiceDependencies){ 0 };
}
