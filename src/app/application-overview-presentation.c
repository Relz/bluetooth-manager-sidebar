#include "app/application-overview-presentation.h"

const char *
application_overview_presentation_status_group_title(void)
{
  return "Status";
}

const char *
application_overview_presentation_connected_group_title(void)
{
  return "Connected Devices";
}

const char *
application_overview_presentation_paired_group_title(void)
{
  return "Paired Devices";
}

const char *
application_overview_presentation_available_group_title(void)
{
  return "Available Devices";
}

const char *
application_overview_presentation_no_adapter_title(void)
{
  return "No adapter";
}

const char *
application_overview_presentation_discoverable_title(void)
{
  return "Discoverable";
}

const char *
application_overview_presentation_discoverable_subtitle(void)
{
  return "Allow nearby Bluetooth devices to find this computer.";
}

const char *
application_overview_presentation_discoverable_tooltip(void)
{
  return "Allow nearby Bluetooth devices to find this computer";
}

const char *
application_overview_presentation_blocked_power_title(void)
{
  return "Bluetooth is blocked/off";
}

const char *
application_overview_presentation_blocked_power_subtitle(void)
{
  return "Unblock Bluetooth in system settings, rfkill, or hardware controls before using this adapter.";
}

const char *
application_overview_presentation_scan_button_label(gboolean discovering)
{
  return discovering ? "Stop" : "Scan";
}

const char *
application_overview_presentation_scan_button_tooltip(void)
{
  return "Start or stop discovery";
}

const char *
application_overview_presentation_available_empty_title(gboolean powered, gboolean discovering)
{
  if (discovering)
    return "Scanning for devices...";
  return powered ? "No available devices" : "Bluetooth is off";
}

const char *
application_overview_presentation_available_empty_subtitle(gboolean powered, gboolean discovering)
{
  if (discovering)
    return "Nearby Bluetooth devices will appear here as they are found.";
  return powered ? "Scan to discover nearby Bluetooth devices." :
                    "Scan to turn Bluetooth on and discover nearby devices.";
}

guint
application_overview_presentation_device_actions(const OverviewDeviceRowModel *model,
                                                OverviewDeviceActionModel *actions,
                                                guint max_actions)
{
  guint count = 0;

  if (!model || !actions || max_actions == 0)
    return 0;

  if (model->paired && !model->trusted && count < max_actions) {
    actions[count++] = (OverviewDeviceActionModel){
      .action = APPLICATION_OVERVIEW_DEVICE_ACTION_TRUST,
      .icon_name = "emblem-ok-symbolic",
      .tooltip = "Trust Device",
    };
  }

  if (model->paired && count < max_actions) {
    actions[count++] = (OverviewDeviceActionModel){
      .action = APPLICATION_OVERVIEW_DEVICE_ACTION_SETTINGS,
      .icon_name = "preferences-system-symbolic",
      .tooltip = "Device Settings",
    };
  }

  return count;
}
