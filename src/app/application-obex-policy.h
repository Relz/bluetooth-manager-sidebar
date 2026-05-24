#pragma once

#include "app/application.h"

#include "obex/obex-service-callbacks.h"

void application_obex_policy_init_dependencies(Application *self,
                                               ObexPolicyProvider *policy,
                                               ObexDeviceResolver *device_resolver);
