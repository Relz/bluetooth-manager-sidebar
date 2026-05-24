#pragma once

#include "bluez/bluez-state.h"

typedef void (*BluezDevicePairStepFinished)(const char *device_path,
                                              gboolean success,
                                              const GError *error,
                                              gpointer user_data);

gboolean bluez_device_pair_step_start(BluezState *state,
                                      const char *device_path,
                                      BluezDevicePairStepFinished finished,
                                      gpointer user_data,
                                      GDestroyNotify destroy);
