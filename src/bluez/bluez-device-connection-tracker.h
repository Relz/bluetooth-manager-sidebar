#pragma once

#include <glib.h>

typedef struct _BluezDeviceConnectionTracker BluezDeviceConnectionTracker;

BluezDeviceConnectionTracker *bluez_device_connection_tracker_new(void);
void bluez_device_connection_tracker_free(BluezDeviceConnectionTracker *tracker);
void bluez_device_connection_tracker_clear(BluezDeviceConnectionTracker *tracker);
void bluez_device_connection_tracker_add(BluezDeviceConnectionTracker *tracker,
                                         const char *device_path);
void bluez_device_connection_tracker_remove(BluezDeviceConnectionTracker *tracker,
                                            const char *device_path);
gboolean bluez_device_connection_tracker_contains(BluezDeviceConnectionTracker *tracker,
                                                  const char *device_path);
