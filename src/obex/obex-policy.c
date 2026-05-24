#include "obex/obex-policy.h"

#include "obex/obex-service-components.h"
#include "obex/obex-service-context.h"
#include "obex/obex-service-policy-private.h"
#include "obex/obex-service-private.h"

static ObexServiceContext *
obex_service_policy_context(ObexService *service)
{
  return obex_service_components_context(obex_service_components(service));
}

static const ObexPolicyProvider *
obex_service_policy_provider(ObexService *service)
{
  return obex_service_context_policy_provider(obex_service_policy_context(service));
}

static const ObexDeviceResolver *
obex_service_device_resolver(ObexService *service)
{
  return obex_service_context_device_resolver(obex_service_policy_context(service));
}

static gpointer
obex_service_callback_user_data(ObexService *service)
{
  return obex_service_context_user_data(obex_service_policy_context(service));
}

char *
obex_policy_dup_received_directory(const ObexPolicyProvider *policy, gpointer user_data)
{
  if (policy && policy->dup_received_directory)
    return policy->dup_received_directory(user_data);

  return g_strdup("");
}

gboolean
obex_policy_auto_accept_trusted(const ObexPolicyProvider *policy,
                                const ObexDeviceResolver *resolver,
                                     gpointer user_data,
                                     const char *address)
{
  if (!policy || !resolver || !address || !*address || !policy->auto_accept_trusted_files ||
      !resolver->is_trusted_address)
    return FALSE;

  return policy->auto_accept_trusted_files(user_data) &&
         resolver->is_trusted_address(address, user_data);
}

char *
obex_policy_dup_device_name(const ObexDeviceResolver *resolver,
                                 gpointer user_data,
                                 const char *address,
                                 const char *fallback)
{
  if (resolver && resolver->dup_device_name_for_address)
    return resolver->dup_device_name_for_address(address, fallback, user_data);

  return g_strdup(fallback ? fallback : "Bluetooth device");
}

char *
obex_service_dup_received_directory(ObexService *service)
{
  return obex_policy_dup_received_directory(obex_service_policy_provider(service),
                                             obex_service_callback_user_data(service));
}

char *
obex_service_dup_device_address(ObexService *service, const char *device_path)
{
  const ObexDeviceResolver *resolver = obex_service_device_resolver(service);

  if (resolver && resolver->dup_device_address)
    return resolver->dup_device_address(device_path, obex_service_callback_user_data(service));

  return NULL;
}

gboolean
obex_service_auto_accept_trusted(ObexService *service, const char *address)
{
  return obex_policy_auto_accept_trusted(obex_service_policy_provider(service),
                                         obex_service_device_resolver(service),
                                          obex_service_callback_user_data(service),
                                          address);
}

char *
obex_service_dup_device_name(ObexService *service, const char *address, const char *fallback)
{
  return obex_policy_dup_device_name(obex_service_device_resolver(service),
                                      obex_service_callback_user_data(service),
                                      address,
                                      fallback);
}
