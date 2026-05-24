#pragma once

#include <adwaita.h>

typedef void (*ApplicationDeviceRemovalConfirmed)(const char *device_path,
                                                     gpointer user_data);

void application_device_removal_dialog_present(GtkWindow *window,
                                               const char *device_path,
                                               const char *device_name,
                                               ApplicationDeviceRemovalConfirmed confirmed,
                                               gpointer user_data,
                                               GDestroyNotify destroy);
