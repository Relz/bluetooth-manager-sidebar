#pragma once

#include <gio/gio.h>

#include "bluez/bluez-operation-reporter.h"

void bluez_device_connection_report_connecting(BluezOperationReporter *reporter);
void bluez_device_connection_report_disconnecting(BluezOperationReporter *reporter);
void bluez_device_connection_report_connected(BluezOperationReporter *reporter,
                                              const char *device_path);
void bluez_device_connection_report_disconnected(BluezOperationReporter *reporter,
                                                 const char *device_path);
void bluez_device_connection_report_connect_failed(BluezOperationReporter *reporter,
                                                   const char *device_path,
                                                   const GError *error);
void bluez_device_connection_report_disconnect_failed(BluezOperationReporter *reporter,
                                                      const char *device_path,
                                                      const GError *error);
void bluez_device_connection_report_connect_unavailable(BluezOperationReporter *reporter,
                                                        const char *device_path);
void bluez_device_connection_report_disconnect_unavailable(BluezOperationReporter *reporter,
                                                           const char *device_path);
void bluez_device_connection_report_already_in_progress(BluezOperationReporter *reporter,
                                                        const char *device_path);
