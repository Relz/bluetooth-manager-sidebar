#include "app/application-send-file-dialog.h"

typedef struct {
  char *device_path;
  ApplicationFileSelected selected;
  ApplicationFileDialogError error;
  gpointer user_data;
  GDestroyNotify destroy;
} SendFileDialog;

static void
send_file_dialog_free(SendFileDialog *dialog)
{
  if (!dialog)
    return;

  if (dialog->destroy && dialog->user_data)
    dialog->destroy(dialog->user_data);
  g_free(dialog->device_path);
  g_free(dialog);
}

static gboolean
send_file_dialog_error_was_dismissed(const GError *error)
{
  return error &&
         (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) ||
          g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) ||
          g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED));
}

static void
on_send_file_dialog_opened(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  SendFileDialog *dialog = user_data;
  g_autoptr(GError) error = NULL;
  g_autoptr(GFile) file = NULL;
  g_autofree char *path = NULL;

  file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source_object), result, &error);
  if (!file) {
    if (!send_file_dialog_error_was_dismissed(error) && dialog->error) {
      const char *detail = error && error->message ? error->message : "unknown error";
      g_autofree char *message = g_strdup_printf("Unable to open file chooser: %s", detail);

      dialog->error(message, dialog->user_data);
    }
    send_file_dialog_free(dialog);
    return;
  }

  path = g_file_get_path(file);
  if (!path) {
    if (dialog->error)
      dialog->error("Only local files can be sent over Bluetooth", dialog->user_data);
    send_file_dialog_free(dialog);
    return;
  }

  if (dialog->selected)
    dialog->selected(dialog->device_path, path, dialog->user_data);
  send_file_dialog_free(dialog);
}

void
application_send_file_dialog_present(GtkWindow *window,
                                     const char *device_path,
                                     ApplicationFileSelected selected,
                                     ApplicationFileDialogError error,
                                     gpointer user_data,
                                     GDestroyNotify destroy)
{
  GtkFileDialog *dialog;
  SendFileDialog *dialog_data;

  if (!device_path) {
    if (destroy && user_data)
      destroy(user_data);
    return;
  }

  (void)window;

  dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_title(dialog, "Send Bluetooth File");
  gtk_file_dialog_set_accept_label(dialog, "Send file");
  gtk_file_dialog_set_modal(dialog, FALSE);

  dialog_data = g_new0(SendFileDialog, 1);
  dialog_data->device_path = g_strdup(device_path);
  dialog_data->selected = selected;
  dialog_data->error = error;
  dialog_data->user_data = user_data;
  dialog_data->destroy = destroy;

  gtk_file_dialog_open(dialog, NULL, NULL, on_send_file_dialog_opened, dialog_data);
  g_object_unref(dialog);
}
