#include "bluez/bluez-device-power-adapter.h"

gboolean
bluez_device_power_adapter_request_on(BluezState *state,
                                      BluezOperationReporter *reporter,
                                      BluezAdapterController *adapter,
                                      const char *adapter_path,
                                      BluezAdapterPowerFinished finished,
                                      gpointer user_data,
                                      GDestroyNotify destroy)
{
  return bluez_adapter_power_workflow_set_power_for_adapter(state,
                                                           reporter,
                                                           adapter,
                                                           adapter_path,
                                                           TRUE,
                                                           finished,
                                                           user_data,
                                                           destroy);
}
