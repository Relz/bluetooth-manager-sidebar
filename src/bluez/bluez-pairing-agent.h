#pragma once

#include <gio/gio.h>

#include "pairing/pairing-prompt-session.h"

typedef struct _BluezPairingAgent BluezPairingAgent;

typedef struct {
  void (*set_agent_message)(const char *message, gpointer user_data);
  char *(*device_name)(const char *device_path, gpointer user_data);
  void (*show_info)(const char *message, gpointer user_data);
  PairingPromptHandle *(*request_confirmation)(const char *heading,
                                                  const char *body,
                                                  PairingAccept accept,
                                                  PairingReject reject,
                                                  gpointer response_data,
                                                  GDestroyNotify response_destroy,
                                                  gpointer user_data);
  PairingPromptHandle *(*request_pin_code)(const char *heading,
                                              const char *body,
                                              PairingEntryAccept accept,
                                              PairingReject reject,
                                              gpointer response_data,
                                              GDestroyNotify response_destroy,
                                              gpointer user_data);
  PairingPromptHandle *(*request_passkey)(const char *heading,
                                             const char *body,
                                             PairingEntryAccept accept,
                                             PairingReject reject,
                                             gpointer response_data,
                                             GDestroyNotify response_destroy,
                                             gpointer user_data);
  gpointer user_data;
} BluezPairingAgentCallbacks;

BluezPairingAgent *bluez_pairing_agent_new(GDBusConnection *connection,
                                                  const BluezPairingAgentCallbacks *callbacks);
void bluez_pairing_agent_free(BluezPairingAgent *agent);
gboolean bluez_pairing_agent_register(BluezPairingAgent *agent);
void bluez_pairing_agent_unregister(BluezPairingAgent *agent);
