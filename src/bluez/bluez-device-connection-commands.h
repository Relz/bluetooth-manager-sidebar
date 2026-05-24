#pragma once

#include <glib.h>

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDeviceConnectionCommands BluezDeviceConnectionCommands;

typedef void (*BluezDeviceConnectionCommandFinished)(const char *device_path,
                                                       gboolean success,
                                                       gpointer user_data);

BluezDeviceConnectionCommands *bluez_device_connection_commands_new(BluezState *state,
                                                                       BluezOperationReporter *reporter);
void bluez_device_connection_commands_free(BluezDeviceConnectionCommands *commands);

gboolean bluez_device_connection_commands_connect(BluezDeviceConnectionCommands *commands,
                                                  const char *device_path,
                                                  BluezDeviceConnectionCommandFinished finished,
                                                  gpointer user_data,
                                                  GDestroyNotify destroy);
gboolean bluez_device_connection_commands_disconnect(BluezDeviceConnectionCommands *commands,
                                                     const char *device_path);
