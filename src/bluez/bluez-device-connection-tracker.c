#include "bluez/bluez-device-connection-tracker.h"

struct _BluezDeviceConnectionTracker {
  GHashTable *device_paths;
};

BluezDeviceConnectionTracker *
bluez_device_connection_tracker_new(void)
{
  BluezDeviceConnectionTracker *tracker = g_new0(BluezDeviceConnectionTracker, 1);

  tracker->device_paths = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  return tracker;
}

void
bluez_device_connection_tracker_free(BluezDeviceConnectionTracker *tracker)
{
  if (!tracker)
    return;

  g_clear_pointer(&tracker->device_paths, g_hash_table_destroy);
  g_free(tracker);
}

void
bluez_device_connection_tracker_clear(BluezDeviceConnectionTracker *tracker)
{
  if (tracker && tracker->device_paths)
    g_hash_table_remove_all(tracker->device_paths);
}

void
bluez_device_connection_tracker_add(BluezDeviceConnectionTracker *tracker,
                                    const char *device_path)
{
  if (tracker && tracker->device_paths && device_path)
    g_hash_table_add(tracker->device_paths, g_strdup(device_path));
}

void
bluez_device_connection_tracker_remove(BluezDeviceConnectionTracker *tracker,
                                       const char *device_path)
{
  if (tracker && tracker->device_paths && device_path)
    g_hash_table_remove(tracker->device_paths, device_path);
}

gboolean
bluez_device_connection_tracker_contains(BluezDeviceConnectionTracker *tracker,
                                         const char *device_path)
{
  return tracker && tracker->device_paths && device_path &&
    g_hash_table_contains(tracker->device_paths, device_path);
}
