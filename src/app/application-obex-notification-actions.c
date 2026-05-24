#include "app/application-obex-notification-actions.h"

#include "app/application-obex-service-access.h"
#include "obex/obex-service-transfer.h"

static void
application_handle_file_notification_action(Application *self,
                                            GVariant *parameter,
                                            gboolean receive)
{
  const char *transfer_path = parameter ? g_variant_get_string(parameter, NULL) : NULL;
  ObexService *obex_service = application_services_obex_service(self);

  if (obex_service && transfer_path)
    obex_service_respond_to_file_notification(obex_service, transfer_path, receive);
}

static void
on_receive_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
  (void)action;
  application_handle_file_notification_action(APPLICATION(user_data), parameter, TRUE);
}

static void
on_reject_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
  (void)action;
  application_handle_file_notification_action(APPLICATION(user_data), parameter, FALSE);
}

void
application_obex_notification_actions_install(Application *self)
{
  const GActionEntry entries[] = {
    {
      .name = "receive-file",
      .activate = on_receive_file_action,
      .parameter_type = "s",
    },
    {
      .name = "reject-file",
      .activate = on_reject_file_action,
      .parameter_type = "s",
    },
  };

  g_action_map_add_action_entries(G_ACTION_MAP(self), entries, G_N_ELEMENTS(entries), self);
}
