#include "app/application-obex-policy.h"

#include "app/application-obex-policy-resolver.h"

static char *
application_obex_dup_received_directory(gpointer user_data)
{
  return application_obex_policy_resolver_dup_received_directory(APPLICATION(user_data));
}

static gboolean
application_obex_auto_accept_trusted_files(gpointer user_data)
{
  return application_obex_policy_resolver_auto_accept_trusted_files(APPLICATION(user_data));
}

static char *
application_obex_dup_device_address(const char *device_path, gpointer user_data)
{
  return application_obex_policy_resolver_dup_device_address(APPLICATION(user_data), device_path);
}

static gboolean
application_obex_is_trusted_address(const char *address, gpointer user_data)
{
  return application_obex_policy_resolver_is_trusted_address(APPLICATION(user_data), address);
}

static char *
application_obex_dup_device_name_for_address(const char *address,
                                             const char *fallback,
                                             gpointer user_data)
{
  return application_obex_policy_resolver_dup_device_name_for_address(APPLICATION(user_data),
                                                                      address,
                                                                      fallback);
}

void
application_obex_policy_init_dependencies(Application *self,
                                          ObexPolicyProvider *policy,
                                          ObexDeviceResolver *device_resolver)
{
  g_return_if_fail(IS_APPLICATION(self));
  g_return_if_fail(policy != NULL);
  g_return_if_fail(device_resolver != NULL);

  policy->dup_received_directory = application_obex_dup_received_directory;
  policy->auto_accept_trusted_files = application_obex_auto_accept_trusted_files;
  device_resolver->dup_device_address = application_obex_dup_device_address;
  device_resolver->is_trusted_address = application_obex_is_trusted_address;
  device_resolver->dup_device_name_for_address = application_obex_dup_device_name_for_address;
}
