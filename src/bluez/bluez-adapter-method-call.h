#pragma once

#include <glib.h>

#include "bluez/bluez-state.h"

typedef enum {
  BLUEZ_ADAPTER_METHOD_CALL_START_DISCOVERY,
  BLUEZ_ADAPTER_METHOD_CALL_STOP_DISCOVERY,
  BLUEZ_ADAPTER_METHOD_CALL_REMOVE_DEVICE,
} BluezAdapterMethodCall;

typedef void (*BluezAdapterMethodCallFinished)(BluezAdapterMethodCall call,
                                                 const char *adapter_path,
                                                 gboolean success,
                                                 const GError *error,
                                                 gpointer user_data);

gboolean bluez_adapter_method_call_start(BluezState *state,
                                         const char *adapter_path,
                                         BluezAdapterMethodCall call,
                                         GVariant *parameters,
                                         BluezAdapterMethodCallFinished finished,
                                         gpointer user_data,
                                         GDestroyNotify destroy);
