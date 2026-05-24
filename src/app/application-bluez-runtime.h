#pragma once

#include "app/application.h"
#include "bluez/bluez-client.h"

typedef struct _ApplicationBluezRuntime ApplicationBluezRuntime;

ApplicationBluezRuntime *application_bluez_runtime_new(Application *self);
gboolean application_bluez_runtime_start(ApplicationBluezRuntime *runtime);
void application_bluez_runtime_free(ApplicationBluezRuntime *runtime);

BluezClient *application_bluez_runtime_client(ApplicationBluezRuntime *runtime);
