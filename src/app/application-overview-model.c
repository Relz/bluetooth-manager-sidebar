#include "app/application-overview-model.h"

#include "app/application-data-access.h"
#include "app/application-device-model.h"
#include "app/application-device-presentation.h"
#include "app/application-page-status.h"
#include "bluez/bluez-client-device.h"
#include "bluez/bluez-client-pairing.h"
#include "bluez/bluez-client-query.h"
#include "bluez/bluez-adapter-power-state.h"
#include "obex/obex-service-status.h"

typedef struct {
  BluezProperties *device;
  gboolean connected;
  gboolean paired;
  gboolean trusted;
  gboolean blocked;
  gboolean connecting;
} OverviewDeviceState;

static OverviewStatusModel
application_overview_status_model_new(BluezClient *bluez,
                                      ObexService *obex_service)
{
  return (OverviewStatusModel){
    .pairing_message = bluez ? bluez_client_pairing_agent_message(bluez) : NULL,
    .obex_message = obex_service ? obex_service_status_message(obex_service) : NULL,
  };
}

static GPtrArray *
application_overview_collect_devices(Application *self,
                                     BluezClient *bluez,
                                     const char *adapter_path,
                                     ApplicationDeviceGroup group)
{
  return application_device_model_collect_devices(self, bluez, adapter_path, group);
}

OverviewModel
application_overview_model_new(Application *self,
                               BluezClient *bluez,
                               ObexService *obex_service)
{
  const char *adapter_path = bluez ? bluez_client_primary_adapter(bluez) : NULL;
  BluezProperties *adapter = application_page_adapter_properties(self, adapter_path);

  return (OverviewModel){
    .adapter_path = adapter_path,
    .adapter = adapter,
    .powered = bluez_properties_get_bool(adapter, "Powered", FALSE),
    .discoverable = bluez_properties_get_bool(adapter, "Discoverable", FALSE),
    .discovering = bluez_properties_get_bool(adapter, "Discovering", FALSE),
    .status = application_overview_status_model_new(bluez, obex_service),
    .connected_devices = application_overview_collect_devices(self,
                                                              bluez,
                                                              adapter_path,
                                                              APPLICATION_DEVICE_GROUP_CONNECTED),
    .paired_devices = application_overview_collect_devices(self,
                                                           bluez,
                                                           adapter_path,
                                                           APPLICATION_DEVICE_GROUP_PAIRED),
    .available_devices = application_overview_collect_devices(self,
                                                              bluez,
                                                              adapter_path,
                                                              APPLICATION_DEVICE_GROUP_AVAILABLE),
  };
}

void
application_overview_model_clear(OverviewModel *model)
{
  if (!model)
    return;

  g_clear_pointer(&model->connected_devices, g_ptr_array_unref);
  g_clear_pointer(&model->paired_devices, g_ptr_array_unref);
  g_clear_pointer(&model->available_devices, g_ptr_array_unref);
}

gboolean
application_overview_model_has_blocked_power_status(const OverviewModel *model)
{
  return model && bluez_adapter_power_state_is_blocked(model->adapter);
}

static OverviewDeviceState
application_overview_device_state_new(Application *self, const char *device_path)
{
  BluezProperties *device = application_device_properties(self, device_path);

  return (OverviewDeviceState){
    .device = device,
    .connected = bluez_properties_get_bool(device, "Connected", FALSE),
    .paired = bluez_properties_get_bool(device, "Paired", FALSE),
    .trusted = bluez_properties_get_bool(device, "Trusted", FALSE),
    .blocked = bluez_properties_get_bool(device, "Blocked", FALSE),
    .connecting = FALSE,
  };
}

static void
application_overview_device_state_set_connecting(OverviewDeviceState *state,
                                                 BluezClient *bluez,
                                                 const char *device_path)
{
  if (state)
    state->connecting = bluez && bluez_client_is_device_connecting(bluez, device_path);
}

OverviewDeviceRowModel
application_overview_device_row_model_new(Application *self,
                                          BluezClient *bluez,
                                          const char *device_path)
{
  OverviewDeviceState state = application_overview_device_state_new(self, device_path);

  application_overview_device_state_set_connecting(&state, bluez, device_path);

  return (OverviewDeviceRowModel){
    .device = state.device,
    .connected = state.connected,
    .paired = state.paired,
    .trusted = state.trusted,
    .blocked = state.blocked,
    .connecting = state.connecting,
    .title = application_device_presentation_dup_display_name(self, device_path),
    .subtitle = application_device_presentation_dup_subtitle(self, device_path),
  };
}

void
application_overview_device_row_model_clear(OverviewDeviceRowModel *model)
{
  if (!model)
    return;

  g_clear_pointer(&model->title, g_free);
  g_clear_pointer(&model->subtitle, g_free);
}
