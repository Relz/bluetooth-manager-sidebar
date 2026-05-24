#include "app/application-device-removal-dialog.h"

#include "core/display.h"

typedef struct {
  char *device_path;
  ApplicationDeviceRemovalConfirmed confirmed;
  gpointer user_data;
  GDestroyNotify destroy;
} DeviceRemovalDialog;

static void
device_removal_dialog_free(DeviceRemovalDialog *dialog)
{
  if (!dialog)
    return;

  if (dialog->destroy && dialog->user_data)
    dialog->destroy(dialog->user_data);
  g_free(dialog->device_path);
  g_free(dialog);
}

static void
on_device_removal_dialog_chosen(GObject *source_object,
                                GAsyncResult *result,
                                gpointer user_data)
{
  DeviceRemovalDialog *dialog = user_data;
  const char *response = adw_alert_dialog_choose_finish(ADW_ALERT_DIALOG(source_object), result);

  if (g_strcmp0(response, "remove") == 0 && dialog->confirmed)
    dialog->confirmed(dialog->device_path, dialog->user_data);

  device_removal_dialog_free(dialog);
}

void
application_device_removal_dialog_present(GtkWindow *window,
                                          const char *device_path,
                                          const char *device_name,
                                          ApplicationDeviceRemovalConfirmed confirmed,
                                          gpointer user_data,
                                          GDestroyNotify destroy)
{
  DeviceRemovalDialog *dialog_data;
  AdwAlertDialog *dialog;
  g_autofree char *safe_device_name = NULL;
  g_autofree char *heading = NULL;

  if (!window || !device_path) {
    if (destroy && user_data)
      destroy(user_data);
    return;
  }

  safe_device_name = display_dup_safe_text(device_name, "Unknown device");
  heading = g_strdup_printf("Remove %s?", safe_device_name ? safe_device_name : "Unknown device");
  dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new(heading,
                                                  "This removes the Bluetooth device from this adapter."));
  gtk_widget_add_css_class(GTK_WIDGET(dialog), "bm-sidebar-dialog");
  adw_alert_dialog_add_response(dialog, "cancel", "Cancel");
  adw_alert_dialog_add_response(dialog, "remove", "Remove");
  adw_alert_dialog_set_close_response(dialog, "cancel");
  adw_alert_dialog_set_response_appearance(dialog, "remove", ADW_RESPONSE_DESTRUCTIVE);

  dialog_data = g_new0(DeviceRemovalDialog, 1);
  dialog_data->device_path = g_strdup(device_path);
  dialog_data->confirmed = confirmed;
  dialog_data->user_data = user_data;
  dialog_data->destroy = destroy;

  adw_alert_dialog_choose(dialog,
                          GTK_WIDGET(window),
                          NULL,
                          on_device_removal_dialog_chosen,
                          dialog_data);
}
