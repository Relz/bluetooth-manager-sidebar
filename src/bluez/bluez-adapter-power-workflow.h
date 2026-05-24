#pragma once

#include <glib.h>

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

gboolean bluez_adapter_power_workflow_set_power(BluezState *state,
                                                BluezOperationReporter *reporter,
                                                BluezAdapterController *controller,
                                                gboolean powered);
gboolean bluez_adapter_power_workflow_set_power_for_adapter(BluezState *state,
                                                            BluezOperationReporter *reporter,
                                                            BluezAdapterController *controller,
                                                            const char *adapter_path,
                                                            gboolean powered,
                                                            BluezAdapterPowerFinished finished,
                                                            gpointer user_data,
                                                            GDestroyNotify destroy);
