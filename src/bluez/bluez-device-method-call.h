#pragma once

#include <glib.h>

#include "bluez/bluez-state.h"

typedef enum {
  BLUEZ_DEVICE_METHOD_CALL_CONNECT,
  BLUEZ_DEVICE_METHOD_CALL_DISCONNECT,
  BLUEZ_DEVICE_METHOD_CALL_PAIR,
} BluezDeviceMethodCall;

typedef void (*BluezDeviceMethodCallFinished)(BluezDeviceMethodCall call,
                                                const char *device_path,
                                                gboolean success,
                                                const GError *error,
                                                gpointer user_data);

gboolean bluez_device_method_call_start(BluezState *state,
                                        const char *device_path,
                                        BluezDeviceMethodCall call,
                                        BluezDeviceMethodCallFinished finished,
                                        gpointer user_data,
                                        GDestroyNotify destroy);
