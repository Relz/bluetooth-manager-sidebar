#include "app/application-obex-adapter.h"

#include "app/application-obex-policy.h"
#include "app/application-pages.h"

static void
on_obex_notify(const char *message, gpointer user_data)
{
  application_show_toast(APPLICATION(user_data), message);
}

static void
on_obex_refresh(gpointer user_data)
{
  application_schedule_signal_refresh(APPLICATION(user_data));
}

static void
on_obex_send_notification(const char *id, GNotification *notification, gpointer user_data)
{
  g_application_send_notification(G_APPLICATION(user_data), id, notification);
}

static void
on_obex_withdraw_notification(const char *id, gpointer user_data)
{
  g_application_withdraw_notification(G_APPLICATION(user_data), id);
}

void
application_obex_adapter_init_dependencies(Application *self, ObexServiceDependencies *dependencies)
{
  *dependencies = (ObexServiceDependencies){ 0 };
  application_obex_policy_init_dependencies(self,
                                            &dependencies->policy,
                                            &dependencies->device_resolver);
  dependencies->status.notify = on_obex_notify;
  dependencies->status.refresh = on_obex_refresh;
  dependencies->notifications.send_notification = on_obex_send_notification;
  dependencies->notifications.withdraw_notification = on_obex_withdraw_notification;
}
