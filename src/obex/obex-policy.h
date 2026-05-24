#pragma once

#include "obex/obex-service-callbacks.h"

char *obex_policy_dup_received_directory(const ObexPolicyProvider *policy,
                                              gpointer user_data);
gboolean obex_policy_auto_accept_trusted(const ObexPolicyProvider *policy,
                                             const ObexDeviceResolver *resolver,
                                              gpointer user_data,
                                              const char *address);
char *obex_policy_dup_device_name(const ObexDeviceResolver *resolver,
                                       gpointer user_data,
                                       const char *address,
                                       const char *fallback);
