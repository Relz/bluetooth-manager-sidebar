#pragma once

#include <glib.h>

typedef enum {
  BLUEZ_DEVICE_PROPERTY_OPERATION_TRUST,
  BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS,
  BLUEZ_DEVICE_PROPERTY_OPERATION_TRUSTED,
  BLUEZ_DEVICE_PROPERTY_OPERATION_BLOCKED,
} BluezDevicePropertyOperation;

const char *bluez_device_property_operation_name(BluezDevicePropertyOperation operation);
const char *bluez_device_property_operation_property_name(BluezDevicePropertyOperation operation);
gboolean bluez_device_property_operation_reports_value(BluezDevicePropertyOperation operation);
const char *bluez_device_property_operation_status_message(BluezDevicePropertyOperation operation,
                                                          gboolean value);
const char *bluez_device_property_operation_success_message(BluezDevicePropertyOperation operation,
                                                           gboolean value);
char *bluez_device_property_operation_failure_message(BluezDevicePropertyOperation operation,
                                                      const char *detail);
