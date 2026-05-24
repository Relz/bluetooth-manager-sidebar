#pragma once

#include <adwaita.h>

typedef void (*ApplicationFileSelected)(const char *device_path,
                                           const char *file_path,
                                           gpointer user_data);
typedef void (*ApplicationFileDialogError)(const char *message, gpointer user_data);

void application_send_file_dialog_present(GtkWindow *window,
                                          const char *device_path,
                                          ApplicationFileSelected selected,
                                          ApplicationFileDialogError error,
                                          gpointer user_data,
                                          GDestroyNotify destroy);
