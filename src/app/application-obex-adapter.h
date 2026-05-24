#pragma once

#include "app/application.h"

#include "obex/obex-service-callbacks.h"

void application_obex_adapter_init_dependencies(Application *self,
                                                ObexServiceDependencies *dependencies);
