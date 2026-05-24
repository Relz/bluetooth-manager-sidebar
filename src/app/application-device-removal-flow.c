#include "app/application-device-removal-flow.h"

#include "app/application-bluetooth-commands.h"
#include "app/application-data-access.h"
#include "app/application-device-removal-dialog.h"
#include "app/application-window-access.h"
#include "bluez/bluez-device-display.h"

static void
on_remove_device_confirmed(const char *device_path, gpointer user_data)
{
  application_commands_forget_device(APPLICATION(user_data), device_path);
}

void
application_device_removal_flow_present(Application *self, const char *device_path)
{
  GtkWindow *window;
  g_autofree char *name = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  if (!device_path)
    return;

  window = application_ensure_window(self);
  if (!window)
    return;

  name = bluez_object_cache_dup_device_display_name(application_bluez_cache(self),
                                                    device_path,
                                                    "Unknown device");
  application_device_removal_dialog_present(window,
                                            device_path,
                                            name,
                                            on_remove_device_confirmed,
                                            g_object_ref(self),
                                            g_object_unref);
}
