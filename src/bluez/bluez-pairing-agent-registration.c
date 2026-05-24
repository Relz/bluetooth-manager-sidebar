#include "bluez/bluez-pairing-agent-registration.h"

#include "bluez/bluez-pairing-agent-manager.h"
#include "bluez/bluez-pairing-agent-object.h"

struct _BluezPairingAgentRegistration {
  guint ref_count;
  gboolean disposed;

  BluezPairingAgentObject *object;
  gboolean registered_with_bluez;
  guint registration_serial;
  guint pending_registration_serial;

  BluezPairingAgentRegistrationStatus status;
  gpointer status_user_data;
};

typedef struct {
  BluezPairingAgentRegistration *registration;
  guint serial;
} BluezPairingAgentRegistrationCallContext;

static BluezPairingAgentRegistration *bluez_pairing_agent_registration_ref(
  BluezPairingAgentRegistration *registration);
static void bluez_pairing_agent_registration_unref(BluezPairingAgentRegistration *registration);

static BluezPairingAgentRegistration *
bluez_pairing_agent_registration_ref(BluezPairingAgentRegistration *registration)
{
  registration->ref_count++;
  return registration;
}

static void
bluez_pairing_agent_registration_finalize(BluezPairingAgentRegistration *registration)
{
  g_clear_pointer(&registration->object, bluez_pairing_agent_object_free);
  g_free(registration);
}

static void
bluez_pairing_agent_registration_unref(BluezPairingAgentRegistration *registration)
{
  registration->ref_count--;
  if (registration->ref_count == 0)
    bluez_pairing_agent_registration_finalize(registration);
}

static void
bluez_pairing_agent_registration_call_context_free(
  BluezPairingAgentRegistrationCallContext *context)
{
  if (!context)
    return;

  bluez_pairing_agent_registration_unref(context->registration);
  g_free(context);
}

static BluezPairingAgentRegistrationCallContext *
bluez_pairing_agent_registration_call_context_new(BluezPairingAgentRegistration *registration)
{
  BluezPairingAgentRegistrationCallContext *context = g_new0(
    BluezPairingAgentRegistrationCallContext,
    1);

  context->registration = bluez_pairing_agent_registration_ref(registration);
  context->serial = registration->registration_serial;

  return context;
}

static void
bluez_pairing_agent_registration_status(BluezPairingAgentRegistration *registration,
                                        const char *prefix,
                                        const char *detail)
{
  g_autofree char *message = NULL;

  if (!registration || registration->disposed || !registration->status)
    return;

  message = g_strdup_printf("%s%s", prefix, detail ? detail : "Unknown error");
  registration->status(message, registration->status_user_data);
}

static GDBusConnection *
bluez_pairing_agent_registration_connection(BluezPairingAgentRegistration *registration)
{
  return registration ? bluez_pairing_agent_object_connection(registration->object) : NULL;
}

static void
bluez_pairing_agent_registration_on_request_default_finished(GObject *source_object,
                                                             GAsyncResult *result,
                                                            gpointer user_data)
{
  BluezPairingAgentRegistrationCallContext *context = user_data;
  BluezPairingAgentRegistration *registration = context->registration;
  g_autoptr(GError) error = NULL;
  gboolean success;
  gboolean is_pending_registration = context->serial == registration->pending_registration_serial;

  if (is_pending_registration)
    registration->pending_registration_serial = 0;

  success = bluez_pairing_agent_manager_request_default_agent_finish(G_DBUS_CONNECTION(source_object),
                                                                    result,
                                                                    &error);
  if (!success && !registration->disposed && context->serial == registration->registration_serial) {
    bluez_pairing_agent_registration_status(registration,
                                            "Unable to become default pairing agent: ",
                                            error ? error->message : NULL);
  }

  bluez_pairing_agent_registration_call_context_free(context);
}

static void
bluez_pairing_agent_registration_on_register_finished(GObject *source_object,
                                                     GAsyncResult *result,
                                                     gpointer user_data)
{
  BluezPairingAgentRegistrationCallContext *context = user_data;
  BluezPairingAgentRegistration *registration = context->registration;
  g_autoptr(GError) error = NULL;
  gboolean success;
  gboolean is_pending_registration = context->serial == registration->pending_registration_serial;

  if (is_pending_registration)
    registration->pending_registration_serial = 0;

  success = bluez_pairing_agent_manager_register_agent_finish(G_DBUS_CONNECTION(source_object),
                                                             result,
                                                             &error);
  if (!success) {
    if (!registration->disposed && context->serial == registration->registration_serial) {
      bluez_pairing_agent_registration_status(registration,
                                              "Pairing agent registration failed: ",
                                              error ? error->message : NULL);
    }
    if (is_pending_registration &&
        (registration->disposed || context->serial != registration->registration_serial) &&
        !registration->registered_with_bluez)
      bluez_pairing_agent_object_unexport(registration->object);
    bluez_pairing_agent_registration_call_context_free(context);
    return;
  }

  if (!registration->disposed && context->serial == registration->registration_serial) {
    registration->registered_with_bluez = TRUE;
    bluez_pairing_agent_manager_request_default_agent(
      G_DBUS_CONNECTION(source_object),
      bluez_pairing_agent_registration_on_request_default_finished,
      bluez_pairing_agent_registration_call_context_new(registration));
  } else {
    g_autoptr(GError) unregister_error = NULL;

    if (is_pending_registration &&
        bluez_pairing_agent_manager_unregister_agent(G_DBUS_CONNECTION(source_object), &unregister_error)) {
      registration->registered_with_bluez = FALSE;
      bluez_pairing_agent_object_unexport(registration->object);
    } else if (is_pending_registration) {
      registration->registered_with_bluez = TRUE;
    }
  }

  bluez_pairing_agent_registration_call_context_free(context);
}

BluezPairingAgentRegistration *
bluez_pairing_agent_registration_new(GDBusConnection *connection,
                                     const GDBusInterfaceVTable *vtable,
                                     gpointer object_user_data,
                                     GDestroyNotify object_destroy,
                                     BluezPairingAgentRegistrationStatus status,
                                     gpointer status_user_data)
{
  BluezPairingAgentRegistration *registration = NULL;

  g_return_val_if_fail(G_IS_DBUS_CONNECTION(connection), NULL);
  g_return_val_if_fail(vtable != NULL, NULL);

  registration = g_new0(BluezPairingAgentRegistration, 1);
  registration->ref_count = 1;
  registration->object = bluez_pairing_agent_object_new(connection,
                                                        vtable,
                                                        object_user_data,
                                                        object_destroy);
  registration->status = status;
  registration->status_user_data = status_user_data;

  return registration;
}

void
bluez_pairing_agent_registration_free(BluezPairingAgentRegistration *registration)
{
  if (!registration)
    return;

  if (!registration->disposed) {
    registration->disposed = TRUE;
    bluez_pairing_agent_registration_unregister(registration);
    registration->status = NULL;
    registration->status_user_data = NULL;
  }

  bluez_pairing_agent_registration_unref(registration);
}

gboolean
bluez_pairing_agent_registration_register(BluezPairingAgentRegistration *registration)
{
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail(registration != NULL, FALSE);

  if (registration->disposed || !bluez_pairing_agent_registration_connection(registration))
    return FALSE;

  if (registration->registered_with_bluez || registration->pending_registration_serial != 0)
    return TRUE;

  if (!bluez_pairing_agent_object_export(registration->object, &error)) {
    bluez_pairing_agent_registration_status(registration,
                                            "Pairing agent unavailable: ",
                                            error ? error->message : NULL);
    return FALSE;
  }

  registration->registration_serial++;
  registration->pending_registration_serial = registration->registration_serial;
  bluez_pairing_agent_manager_register_agent(
    bluez_pairing_agent_registration_connection(registration),
    bluez_pairing_agent_registration_on_register_finished,
    bluez_pairing_agent_registration_call_context_new(registration));

  return TRUE;
}

void
bluez_pairing_agent_registration_unregister(BluezPairingAgentRegistration *registration)
{
  g_autoptr(GError) error = NULL;
  gboolean registration_pending;

  if (!registration)
    return;

  registration_pending = registration->pending_registration_serial != 0;
  registration->registration_serial++;

  if (registration->registered_with_bluez && bluez_pairing_agent_registration_connection(registration)) {
    if (bluez_pairing_agent_manager_unregister_agent(bluez_pairing_agent_registration_connection(registration),
                                                     &error)) {
      registration->registered_with_bluez = FALSE;
    } else if (!registration->disposed) {
      bluez_pairing_agent_registration_status(registration,
                                              "Unable to unregister pairing agent: ",
                                              error ? error->message : NULL);
    }
  }

  if (!registration->registered_with_bluez && !registration_pending)
    bluez_pairing_agent_object_unexport(registration->object);
}
