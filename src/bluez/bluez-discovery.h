#pragma once

#include <glib.h>

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDiscoveryController BluezDiscoveryController;

BluezDiscoveryController *bluez_discovery_controller_new(BluezState *state,
                                                           BluezOperationReporter *reporter,
                                                           BluezAdapterController *adapter);
void bluez_discovery_controller_free(BluezDiscoveryController *controller);
void bluez_discovery_controller_shutdown(BluezDiscoveryController *controller);
void bluez_discovery_controller_toggle(BluezDiscoveryController *controller);
void bluez_discovery_controller_start(BluezDiscoveryController *controller);
void bluez_discovery_controller_stop(BluezDiscoveryController *controller,
                                     gboolean report_stopped_message);
