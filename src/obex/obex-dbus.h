#pragma once

#define OBEX_BUS_NAME "org.bluez.obex"
#define OBEX_ROOT_PATH "/org/bluez/obex"
#define OBEX_CLIENT_INTERFACE "org.bluez.obex.Client1"
#define OBEX_AGENT_MANAGER_INTERFACE "org.bluez.obex.AgentManager1"
#define OBEX_AGENT_INTERFACE "org.bluez.obex.Agent1"
#define OBEX_OBJECT_PUSH_INTERFACE "org.bluez.obex.ObjectPush1"
#define OBEX_TRANSFER_INTERFACE "org.bluez.obex.Transfer1"
#define OBEX_SESSION_INTERFACE "org.bluez.obex.Session1"
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define OBEX_ERROR_REJECTED "org.bluez.obex.Error.Rejected"
#define OBEX_ERROR_CANCELED "org.bluez.obex.Error.Canceled"
#define OBEX_REGISTER_TIMEOUT_MS 5000
#define OBEX_UNREGISTER_TIMEOUT_MS 1000
#define OBEX_GET_PROPERTIES_TIMEOUT_MS 1000
#define OBEX_CREATE_SESSION_TIMEOUT_MS 10000
#define OBEX_SEND_FILE_TIMEOUT_MS 30000
#define OBEX_REMOVE_SESSION_TIMEOUT_MS 5000
#define OBEX_INCOMING_NOTIFICATION_ID "incoming-file-transfer"
#define OBEX_COMPLETED_NOTIFICATION_ID "completed-file-transfer"
#define OBEX_OUTGOING_FAILED_NOTIFICATION_ID "outgoing-file-transfer-failed"
