#pragma once

#include <gio/gio.h>

void bluez_pairing_agent_manager_register_agent(GDBusConnection *connection,
                                                GAsyncReadyCallback callback,
                                                gpointer user_data);
gboolean bluez_pairing_agent_manager_register_agent_finish(GDBusConnection *connection,
                                                           GAsyncResult *result,
                                                           GError **error);
void bluez_pairing_agent_manager_request_default_agent(GDBusConnection *connection,
                                                       GAsyncReadyCallback callback,
                                                       gpointer user_data);
gboolean bluez_pairing_agent_manager_request_default_agent_finish(GDBusConnection *connection,
                                                                  GAsyncResult *result,
                                                                  GError **error);
gboolean bluez_pairing_agent_manager_unregister_agent(GDBusConnection *connection,
                                                      GError **error);
