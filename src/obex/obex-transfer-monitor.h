#pragma once

#include <gio/gio.h>

typedef struct _ObexTransferMonitor ObexTransferMonitor;

typedef void (*ObexTransferStatusHandler)(const char *transfer_path,
                                             const char *status,
                                             gpointer user_data);

ObexTransferMonitor *obex_transfer_monitor_new(GDBusConnection *connection,
                                                   ObexTransferStatusHandler handler,
                                                   gpointer user_data);
void obex_transfer_monitor_free(ObexTransferMonitor *monitor);
