#include "app/application-file-actions.h"

#include "app/application-action-data.h"
#include "app/application-file-commands.h"
#include "app/application-send-file-dialog.h"
#include "app/application-window-access.h"

static void
on_send_file_selected(const char *device_path, const char *file_path, gpointer user_data)
{
  application_commands_send_file_to_device(APPLICATION(user_data), device_path, file_path);
}

static void
on_send_file_dialog_error(const char *message, gpointer user_data)
{
  application_show_toast(APPLICATION(user_data), message);
}

void
application_actions_on_send_file_clicked(GtkButton *button, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(button));
  GtkWindow *window;

  if (!device_path)
    return;
  window = application_ensure_window(self);
  if (!window)
    return;

  application_send_file_dialog_present(window,
                                       device_path,
                                       on_send_file_selected,
                                       on_send_file_dialog_error,
                                       g_object_ref(self),
                                       g_object_unref);
}
