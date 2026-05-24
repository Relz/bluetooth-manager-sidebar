#pragma once

#include <gio/gio.h>

typedef char *(*ObexDupReceivedDirectory)(gpointer user_data);
typedef gboolean (*ObexAutoAcceptTrustedFiles)(gpointer user_data);
typedef char *(*ObexDupDeviceAddress)(const char *device_path, gpointer user_data);
typedef gboolean (*ObexIsTrustedAddress)(const char *address, gpointer user_data);
typedef char *(*ObexDupDeviceNameForAddress)(const char *address,
                                                const char *fallback,
                                                gpointer user_data);
typedef void (*ObexNotify)(const char *message, gpointer user_data);
typedef void (*ObexRefresh)(gpointer user_data);
typedef void (*ObexSendNotification)(const char *id,
                                        GNotification *notification,
                                        gpointer user_data);
typedef void (*ObexWithdrawNotification)(const char *id, gpointer user_data);

typedef struct {
  ObexDupReceivedDirectory dup_received_directory;
  ObexAutoAcceptTrustedFiles auto_accept_trusted_files;
} ObexPolicyProvider;

typedef struct {
  ObexDupDeviceAddress dup_device_address;
  ObexIsTrustedAddress is_trusted_address;
  ObexDupDeviceNameForAddress dup_device_name_for_address;
} ObexDeviceResolver;

typedef struct {
  ObexNotify notify;
  ObexRefresh refresh;
} ObexStatusSink;

typedef struct {
  ObexSendNotification send_notification;
  ObexWithdrawNotification withdraw_notification;
} ObexNotificationSink;

typedef struct {
  ObexPolicyProvider policy;
  ObexDeviceResolver device_resolver;
  ObexStatusSink status;
  ObexNotificationSink notifications;
} ObexServiceDependencies;
