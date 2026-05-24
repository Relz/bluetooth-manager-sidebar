#include "obex/obex-service.h"

#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-private.h"
#include "obex/obex-service-lifecycle-steps.h"
#include "obex/obex-service-notification-private.h"
#include "obex/obex-service-transfer.h"

static void
obex_service_on_transfer_status(const char *transfer_path,
                                const char *status,
                                gpointer user_data)
{
  obex_service_handle_transfer_status(user_data, transfer_path, status);
}

gboolean
obex_service_start(ObexService *service)
{
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail(service != NULL, FALSE);
  if (obex_service_is_disposed(service))
    return FALSE;

  if (!obex_service_lifecycle_connect_session_bus(service, &error)) {
    g_autofree char *message = g_strdup_printf("Session bus unavailable: %s",
                                                error ? error->message : "unknown error");

    obex_service_set_status(service, message);
    return FALSE;
  }

  if (!obex_service_lifecycle_register_agent_object(service, &error)) {
    g_autofree char *message = g_strdup_printf("File receive agent unavailable: %s",
                                                error ? error->message : "unknown error");

    obex_service_set_status(service, message);
    return FALSE;
  }

  obex_service_lifecycle_start_transfer_monitor(service,
                                                obex_service_on_transfer_status,
                                                service);

  if (!obex_service_lifecycle_register_agent_with_obex(service, &error)) {
    g_autofree char *message = g_strdup_printf("File receive agent registration failed: %s",
                                                error ? error->message : "unknown error");

    obex_service_set_status(service, message);
    return FALSE;
  }

  obex_service_set_status(service, NULL);
  return TRUE;
}

void
obex_service_shutdown(ObexService *service)
{
  if (!service)
    return;

  if (obex_service_is_disposed(service) && !obex_service_connection(service))
    return;
  obex_service_mark_disposed(service);

  obex_service_lifecycle_cancel_incoming(service);
  obex_service_lifecycle_withdraw_notifications(service);
  obex_service_lifecycle_unregister_agent(service);
  obex_service_lifecycle_stop_transfer_monitor(service);
  obex_service_lifecycle_clear_transfers(service);
  obex_service_lifecycle_clear_agent_state(service);
}
