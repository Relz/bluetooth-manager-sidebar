#include "bluez/bluez-device-property-operation.h"

#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

typedef char *(*BluezDevicePropertyFailureMessage)(const char *detail);

typedef struct {
  BluezDevicePropertyOperation operation;
  const char *operation_name;
  const char *property_name;
  gboolean report_value;
  BluezDevicePropertyFailureMessage failure_message;
} BluezDevicePropertyDescriptor;

static const BluezDevicePropertyDescriptor device_property_descriptors[] = {
  {
    .operation = BLUEZ_DEVICE_PROPERTY_OPERATION_TRUST,
    .operation_name = BLUEZ_OPERATION_TRUST_DEVICE,
    .property_name = "Trusted",
    .report_value = FALSE,
    .failure_message = bluez_operation_message_trust_failed,
  },
  {
    .operation = BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS,
    .operation_name = BLUEZ_OPERATION_SET_DEVICE_ALIAS,
    .property_name = "Alias",
    .report_value = TRUE,
    .failure_message = bluez_operation_message_alias_failed,
  },
  {
    .operation = BLUEZ_DEVICE_PROPERTY_OPERATION_TRUSTED,
    .operation_name = BLUEZ_OPERATION_SET_DEVICE_TRUSTED,
    .property_name = "Trusted",
    .report_value = TRUE,
    .failure_message = bluez_operation_message_trusted_failed,
  },
  {
    .operation = BLUEZ_DEVICE_PROPERTY_OPERATION_BLOCKED,
    .operation_name = BLUEZ_OPERATION_SET_DEVICE_BLOCKED,
    .property_name = "Blocked",
    .report_value = TRUE,
    .failure_message = bluez_operation_message_blocked_failed,
  },
};

static const BluezDevicePropertyDescriptor *
bluez_device_property_descriptor(BluezDevicePropertyOperation operation)
{
  for (guint i = 0; i < G_N_ELEMENTS(device_property_descriptors); i++) {
    if (device_property_descriptors[i].operation == operation)
      return &device_property_descriptors[i];
  }

  return &device_property_descriptors[0];
}

const char *
bluez_device_property_operation_name(BluezDevicePropertyOperation operation)
{
  return bluez_device_property_descriptor(operation)->operation_name;
}

const char *
bluez_device_property_operation_property_name(BluezDevicePropertyOperation operation)
{
  return bluez_device_property_descriptor(operation)->property_name;
}

gboolean
bluez_device_property_operation_reports_value(BluezDevicePropertyOperation operation)
{
  return bluez_device_property_descriptor(operation)->report_value;
}

const char *
bluez_device_property_operation_status_message(BluezDevicePropertyOperation operation,
                                               gboolean value)
{
  switch (operation) {
    case BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS:
      return bluez_operation_message_renaming_device();
    case BLUEZ_DEVICE_PROPERTY_OPERATION_TRUSTED:
      return value ? bluez_operation_message_trusting_device() :
                     bluez_operation_message_untrusting_device();
    case BLUEZ_DEVICE_PROPERTY_OPERATION_BLOCKED:
      return value ? bluez_operation_message_blocking_device() :
                     bluez_operation_message_unblocking_device();
    case BLUEZ_DEVICE_PROPERTY_OPERATION_TRUST:
    default:
      return bluez_operation_message_trusting_device();
  }
}

const char *
bluez_device_property_operation_success_message(BluezDevicePropertyOperation operation,
                                                gboolean value)
{
  switch (operation) {
    case BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS:
      return bluez_operation_message_device_renamed();
    case BLUEZ_DEVICE_PROPERTY_OPERATION_TRUSTED:
      return bluez_operation_message_trusted_changed(value);
    case BLUEZ_DEVICE_PROPERTY_OPERATION_BLOCKED:
      return bluez_operation_message_blocked_changed(value);
    case BLUEZ_DEVICE_PROPERTY_OPERATION_TRUST:
    default:
      return bluez_operation_message_device_trusted();
  }
}

char *
bluez_device_property_operation_failure_message(BluezDevicePropertyOperation operation,
                                                const char *detail)
{
  return bluez_device_property_descriptor(operation)->failure_message(detail);
}
