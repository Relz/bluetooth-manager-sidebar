#pragma once

#include "bluez/bluez-pairing-agent.h"

#include "bluez/bluez-pairing-agent-registration.h"

#define BLUEZ_PAIRING_ERROR_REJECTED "org.bluez.Error.Rejected"
#define BLUEZ_PAIRING_ERROR_NOT_SUPPORTED "org.bluez.Error.NotSupported"
#define PAIRING_REPLACED_MESSAGE "Another pairing request replaced this one"
#define PAIRING_UNAVAILABLE_MESSAGE "Bluetooth Manager Sidebar window unavailable"
#define PAIRING_REJECTED_MESSAGE "Pairing request rejected"
#define PAIRING_CANCELLED_MESSAGE "Bluetooth pairing was cancelled"
#define PAIRING_RELEASED_MESSAGE "Bluetooth pairing agent was released"

typedef struct _PairingResponse PairingResponse;

typedef PairingPromptHandle *(*BluezPairingEntryPrompt)(const char *heading,
                                                             const char *body,
                                                             PairingEntryAccept accept,
                                                             PairingReject reject,
                                                             gpointer response_data,
                                                             GDestroyNotify response_destroy,
                                                             gpointer user_data);

struct _BluezPairingAgent {
  guint ref_count;
  gboolean disposed;

  BluezPairingAgentRegistration *registration;

  BluezPairingAgentCallbacks callbacks;
  PairingPromptSession prompt_session;
};

struct _PairingResponse {
  guint ref_count;
  BluezPairingAgent *agent;
  GDBusMethodInvocation *invocation;
};

BluezPairingAgent *bluez_pairing_agent_ref(BluezPairingAgent *agent);
void bluez_pairing_agent_unref(BluezPairingAgent *agent);
char *bluez_pairing_agent_device_name(BluezPairingAgent *agent, const char *device_path);
void bluez_pairing_agent_show_info(BluezPairingAgent *agent, const char *message);
void bluez_pairing_agent_close_active(BluezPairingAgent *agent,
                                      gboolean reject,
                                      const char *message);

PairingResponse *bluez_pairing_response_new(BluezPairingAgent *agent,
                                               GDBusMethodInvocation *invocation);
gpointer bluez_pairing_response_ref(gpointer user_data);
void bluez_pairing_response_unref(gpointer user_data);
void bluez_pairing_response_accept(gpointer user_data);
void bluez_pairing_response_reject(gpointer user_data);
void bluez_pairing_response_accept_pin_code(const char *text, gpointer user_data);
void bluez_pairing_response_accept_passkey(const char *text, gpointer user_data);

void bluez_pairing_agent_request_confirmation_prompt(BluezPairingAgent *agent,
                                                     const char *heading,
                                                     const char *body,
                                                     GDBusMethodInvocation *invocation);
void bluez_pairing_agent_request_entry_prompt(BluezPairingAgent *agent,
                                              const char *heading,
                                              const char *body,
                                              GDBusMethodInvocation *invocation,
                                              PairingEntryAccept accept,
                                              BluezPairingEntryPrompt request);

void bluez_pairing_agent_display_pin_code(BluezPairingAgent *agent,
                                          const char *device_path,
                                          const char *pin_code);
void bluez_pairing_agent_display_passkey(BluezPairingAgent *agent,
                                         const char *device_path,
                                         guint32 passkey,
                                         guint16 entered);
void bluez_pairing_agent_request_confirmation(BluezPairingAgent *agent,
                                              const char *device_path,
                                              guint32 passkey,
                                              GDBusMethodInvocation *invocation);
void bluez_pairing_agent_request_authorization(BluezPairingAgent *agent,
                                               const char *device_path,
                                               GDBusMethodInvocation *invocation);
void bluez_pairing_agent_authorize_service(BluezPairingAgent *agent,
                                           const char *device_path,
                                           const char *uuid,
                                           GDBusMethodInvocation *invocation);
void bluez_pairing_agent_request_pin_code(BluezPairingAgent *agent,
                                          const char *device_path,
                                          GDBusMethodInvocation *invocation);
void bluez_pairing_agent_request_passkey(BluezPairingAgent *agent,
                                         const char *device_path,
                                         GDBusMethodInvocation *invocation);

const GDBusInterfaceVTable *bluez_pairing_agent_dbus_vtable(void);
