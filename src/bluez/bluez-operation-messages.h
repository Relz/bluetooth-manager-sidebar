#pragma once

#include <gio/gio.h>

const char *bluez_operation_message_no_adapter(void);
const char *bluez_operation_message_system_bus_unavailable(void);
const char *bluez_operation_message_discoverable_requires_power(void);
const char *bluez_operation_message_turning_bluetooth_on(void);
const char *bluez_operation_message_unblocking_bluetooth(void);
char *bluez_operation_message_adapter_unblock_failed(const char *reason,
                                                     const char *detail);
const char *bluez_operation_message_power_on_pending(void);
char *bluez_operation_message_device_adapter_unavailable(const char *action);
char *bluez_operation_message_device_power_failed(const char *action);

const char *bluez_operation_message_scan_requires_power(void);
const char *bluez_operation_message_scan_started(void);
const char *bluez_operation_message_scan_stopped(void);
char *bluez_operation_message_scan_start_failed(const char *detail);
char *bluez_operation_message_scan_stop_failed(const char *detail);

const char *bluez_operation_message_pairing_device(void);
const char *bluez_operation_message_device_paired(void);
char *bluez_operation_message_pair_failed(const char *detail);

const char *bluez_operation_message_removing_device(void);
const char *bluez_operation_message_device_removed(void);
char *bluez_operation_message_remove_failed(const char *detail);

const char *bluez_operation_message_connection_already_in_progress(void);
const char *bluez_operation_message_connecting_device(void);
const char *bluez_operation_message_disconnecting_device(void);
const char *bluez_operation_message_device_connected(void);
const char *bluez_operation_message_device_disconnected(void);
char *bluez_operation_message_connect_failed(const GError *error);
char *bluez_operation_message_disconnect_failed(const GError *error);

const char *bluez_operation_message_trusting_device(void);
const char *bluez_operation_message_untrusting_device(void);
const char *bluez_operation_message_blocking_device(void);
const char *bluez_operation_message_unblocking_device(void);
const char *bluez_operation_message_renaming_device(void);
char *bluez_operation_message_trust_failed(const char *detail);
char *bluez_operation_message_alias_failed(const char *detail);
char *bluez_operation_message_trusted_failed(const char *detail);
char *bluez_operation_message_blocked_failed(const char *detail);
const char *bluez_operation_message_device_trusted(void);
const char *bluez_operation_message_device_renamed(void);
const char *bluez_operation_message_trusted_changed(gboolean trusted);
const char *bluez_operation_message_blocked_changed(gboolean blocked);

char *bluez_operation_message_adapter_power_failed(gboolean powered,
                                                   const char *detail);
const char *bluez_operation_message_adapter_power_succeeded(gboolean powered);
char *bluez_operation_message_adapter_discoverable_failed(gboolean discoverable,
                                                          const char *detail);
const char *bluez_operation_message_adapter_discoverable_succeeded(gboolean discoverable);
