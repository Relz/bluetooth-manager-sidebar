#pragma once

#include "app/application.h"

char *application_obex_policy_resolver_dup_received_directory(Application *self);
gboolean application_obex_policy_resolver_auto_accept_trusted_files(Application *self);
char *application_obex_policy_resolver_dup_device_address(Application *self,
                                                          const char *device_path);
gboolean application_obex_policy_resolver_is_trusted_address(Application *self,
                                                             const char *address);
char *application_obex_policy_resolver_dup_device_name_for_address(Application *self,
                                                                   const char *address,
                                                                   const char *fallback);
