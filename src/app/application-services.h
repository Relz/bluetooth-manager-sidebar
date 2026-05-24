#pragma once

#include "app/application.h"

typedef struct _ApplicationServices ApplicationServices;

gboolean application_services_start(Application *self, GError **error);
gboolean application_services_ensure_command_server(Application *self, GError **error);
void application_services_stop(Application *self);
