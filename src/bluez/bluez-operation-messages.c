#include "bluez/bluez-operation-messages.h"

#include <string.h>

static char *
bluez_operation_error_detail(const GError *error)
{
  char *message = g_strdup((error && error->message && *error->message) ? error->message : "Unknown error");

  g_strstrip(message);
  if (g_str_has_prefix(message, "GDBus.Error:") || g_str_has_prefix(message, "GDBus.Error.")) {
    const char *separator = strstr(message, ": ");

    if (separator && separator[2] != '\0') {
      char *detail = g_strdup(separator + 2);

      g_strstrip(detail);
      if (*detail) {
        g_free(message);
        return detail;
      }
      g_free(detail);
    }
  }

  return message;
}

const char *
bluez_operation_message_no_adapter(void)
{
  return "No Bluetooth adapter available";
}

const char *
bluez_operation_message_system_bus_unavailable(void)
{
  return "System bus unavailable";
}

const char *
bluez_operation_message_discoverable_requires_power(void)
{
  return "Bluetooth must be powered on to be discoverable";
}

const char *
bluez_operation_message_turning_bluetooth_on(void)
{
  return "Turning Bluetooth on...";
}

const char *
bluez_operation_message_unblocking_bluetooth(void)
{
  return "Unblocking Bluetooth...";
}

char *
bluez_operation_message_adapter_unblock_failed(const char *reason, const char *detail)
{
  if (g_strcmp0(reason, "unavailable") == 0)
    return g_strdup("Unable to turn Bluetooth on: rfkill is unavailable");
  if (g_strcmp0(reason, "timeout") == 0)
    return g_strdup("Unable to turn Bluetooth on: rfkill timed out");
  if (detail && *detail)
    return g_strdup_printf("Unable to unblock Bluetooth: %s", detail);

  return g_strdup("Unable to unblock Bluetooth");
}

const char *
bluez_operation_message_power_on_pending(void)
{
  return "Bluetooth is already turning on. Try again in a moment.";
}

char *
bluez_operation_message_device_adapter_unavailable(const char *action)
{
  return g_strdup_printf("Unable to %s device: adapter unavailable",
                         (action && *action) ? action : "operate on");
}

char *
bluez_operation_message_device_power_failed(const char *action)
{
  return g_strdup_printf("Unable to %s device: Bluetooth did not turn on",
                         (action && *action) ? action : "operate on");
}

const char *
bluez_operation_message_scan_requires_power(void)
{
  return "Bluetooth must be powered on to scan";
}

const char *
bluez_operation_message_scan_started(void)
{
  return "Bluetooth scan started";
}

const char *
bluez_operation_message_scan_stopped(void)
{
  return "Bluetooth scan stopped";
}

char *
bluez_operation_message_scan_start_failed(const char *detail)
{
  return g_strdup_printf("Unable to start scan: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

char *
bluez_operation_message_scan_stop_failed(const char *detail)
{
  return g_strdup_printf("Unable to stop scan: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_pairing_device(void)
{
  return "Pairing device...";
}

const char *
bluez_operation_message_device_paired(void)
{
  return "Device paired";
}

char *
bluez_operation_message_pair_failed(const char *detail)
{
  return g_strdup_printf("Unable to pair device: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_removing_device(void)
{
  return "Removing device...";
}

const char *
bluez_operation_message_device_removed(void)
{
  return "Device removed";
}

char *
bluez_operation_message_remove_failed(const char *detail)
{
  return g_strdup_printf("Unable to remove device: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_connection_already_in_progress(void)
{
  return "Connection already in progress";
}

const char *
bluez_operation_message_connecting_device(void)
{
  return "Connecting device...";
}

const char *
bluez_operation_message_disconnecting_device(void)
{
  return "Disconnecting device...";
}

const char *
bluez_operation_message_device_connected(void)
{
  return "Device connected";
}

const char *
bluez_operation_message_device_disconnected(void)
{
  return "Device disconnected";
}

char *
bluez_operation_message_connect_failed(const GError *error)
{
  g_autofree char *detail = bluez_operation_error_detail(error);
  g_autofree char *normalized = g_utf8_casefold(detail, -1);

  if (normalized && strstr(normalized, "operation already in progress"))
    return g_strdup("Connection already in progress. Wait a moment and try again.");

  return g_strdup_printf("Unable to connect device: %s", detail);
}

char *
bluez_operation_message_disconnect_failed(const GError *error)
{
  return g_strdup_printf("Unable to disconnect device: %s",
                         (error && error->message && *error->message) ? error->message : "Unknown error");
}

const char *
bluez_operation_message_trusting_device(void)
{
  return "Trusting device...";
}

const char *
bluez_operation_message_untrusting_device(void)
{
  return "Untrusting device...";
}

const char *
bluez_operation_message_blocking_device(void)
{
  return "Blocking device...";
}

const char *
bluez_operation_message_unblocking_device(void)
{
  return "Unblocking device...";
}

const char *
bluez_operation_message_renaming_device(void)
{
  return "Renaming device...";
}

char *
bluez_operation_message_trust_failed(const char *detail)
{
  return g_strdup_printf("Unable to trust device: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

char *
bluez_operation_message_alias_failed(const char *detail)
{
  return g_strdup_printf("Unable to rename device: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

char *
bluez_operation_message_trusted_failed(const char *detail)
{
  return g_strdup_printf("Unable to update trusted state: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

char *
bluez_operation_message_blocked_failed(const char *detail)
{
  return g_strdup_printf("Unable to update blocked state: %s",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_device_trusted(void)
{
  return "Device trusted";
}

const char *
bluez_operation_message_device_renamed(void)
{
  return "Device renamed";
}

const char *
bluez_operation_message_trusted_changed(gboolean trusted)
{
  return trusted ? "Device trusted" : "Device no longer trusted";
}

const char *
bluez_operation_message_blocked_changed(gboolean blocked)
{
  return blocked ? "Device blocked" : "Device unblocked";
}

char *
bluez_operation_message_adapter_power_failed(gboolean powered, const char *detail)
{
  return g_strdup_printf("Unable to turn Bluetooth %s: %s",
                         powered ? "on" : "off",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_adapter_power_succeeded(gboolean powered)
{
  return powered ? "Bluetooth enabled" : "Bluetooth disabled";
}

char *
bluez_operation_message_adapter_discoverable_failed(gboolean discoverable,
                                                    const char *detail)
{
  return g_strdup_printf("Unable to %s discoverable mode: %s",
                         discoverable ? "enable" : "disable",
                         (detail && *detail) ? detail : "Unknown error");
}

const char *
bluez_operation_message_adapter_discoverable_succeeded(gboolean discoverable)
{
  return discoverable ? "Bluetooth is discoverable" : "Bluetooth is no longer discoverable";
}
