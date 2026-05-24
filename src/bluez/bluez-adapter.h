#pragma once

#include <glib.h>

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezAdapterController BluezAdapterController;

typedef void (*BluezAdapterPowerFinished)(const char *adapter_path,
                                             gboolean success,
                                             gpointer user_data);

BluezAdapterController *bluez_adapter_controller_new(BluezState *state,
                                                             BluezOperationReporter *reporter);
void bluez_adapter_controller_free(BluezAdapterController *controller);
void bluez_adapter_controller_set_power(BluezAdapterController *controller,
                                             gboolean powered);
gboolean bluez_adapter_controller_set_power_for_adapter(BluezAdapterController *controller,
                                                             const char *adapter_path,
                                                             gboolean powered,
                                                             BluezAdapterPowerFinished finished,
                                                             gpointer user_data,
                                                             GDestroyNotify destroy);
void bluez_adapter_controller_set_discoverable(BluezAdapterController *controller,
                                                   gboolean discoverable);
