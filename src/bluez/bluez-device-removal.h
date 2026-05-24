#pragma once

#include <glib.h>

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDeviceRemovalController BluezDeviceRemovalController;

BluezDeviceRemovalController *bluez_device_removal_controller_new(BluezState *state,
                                                                         BluezOperationReporter *reporter);
void bluez_device_removal_controller_free(BluezDeviceRemovalController *controller);
void bluez_device_removal_controller_forget(BluezDeviceRemovalController *controller,
                                                const char *device_path);
