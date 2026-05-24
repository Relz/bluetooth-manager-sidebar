#include "app/application-bluez-adapter.h"

#include "app/application-audio-page-loader.h"
#include "app/application-pages.h"

static void
application_bluez_adapter_show_operation_result(Application *self,
                                                const BluezOperationResult *result)
{
  const char *message = bluez_operation_result_get_message(result);

  if (message && *message)
    application_show_toast(self, message);
  application_schedule_refresh(self, 100, FALSE);
}

static void
on_bluez_state_changed(gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  application_audio_page_loader_load_selected_device(self);
  application_schedule_signal_refresh(self);
}

static void
on_bluez_status(const char *message, gpointer user_data)
{
  application_show_toast(APPLICATION(user_data), message);
}

static void
on_bluez_operation_result(const BluezOperationResult *result, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  application_bluez_adapter_show_operation_result(self, result);
}

void
application_bluez_adapter_init_callbacks(Application *self,
                                         ApplicationBluezCallbacks *callbacks)
{
  g_return_if_fail(IS_APPLICATION(self));
  g_return_if_fail(callbacks != NULL);

  *callbacks = (ApplicationBluezCallbacks){
    .state_changed = on_bluez_state_changed,
    .status = on_bluez_status,
    .operation_result = on_bluez_operation_result,
    .user_data = self,
  };
}
