#pragma once

#include <glib.h>

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDevicePropertiesController BluezDevicePropertiesController;
typedef void (*BluezDevicePropertyFinished)(const char *device_path,
                                               gboolean success,
                                               gpointer user_data);

BluezDevicePropertiesController *bluez_device_properties_controller_new(BluezState *state,
                                                                               BluezOperationReporter *reporter);
void bluez_device_properties_controller_free(BluezDevicePropertiesController *controller);
void bluez_device_properties_controller_trust(BluezDevicePropertiesController *controller,
                                                   const char *device_path);
void bluez_device_properties_controller_trust_with_callback(BluezDevicePropertiesController *controller,
                                                            const char *device_path,
                                                            BluezDevicePropertyFinished finished,
                                                            gpointer user_data,
                                                            GDestroyNotify destroy);
void bluez_device_properties_controller_set_alias(BluezDevicePropertiesController *controller,
                                                      const char *device_path,
                                                      const char *alias);
void bluez_device_properties_controller_set_trusted(BluezDevicePropertiesController *controller,
                                                        const char *device_path,
                                                        gboolean trusted);
void bluez_device_properties_controller_set_blocked(BluezDevicePropertiesController *controller,
                                                        const char *device_path,
                                                        gboolean blocked);
