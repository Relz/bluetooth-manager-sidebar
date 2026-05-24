#pragma once

#include "app/application.h"
#include "bluez/bluez-client.h"
#include "bluez/bluez-properties.h"
#include "obex/obex-service.h"

typedef struct {
  const char *pairing_message;
  const char *obex_message;
} OverviewStatusModel;

typedef struct {
  const char *adapter_path;
  BluezProperties *adapter;
  gboolean powered;
  gboolean discoverable;
  gboolean discovering;
  OverviewStatusModel status;
  GPtrArray *connected_devices;
  GPtrArray *paired_devices;
  GPtrArray *available_devices;
} OverviewModel;

typedef struct {
  BluezProperties *device;
  gboolean connected;
  gboolean paired;
  gboolean trusted;
  gboolean blocked;
  gboolean connecting;
  char *title;
  char *subtitle;
} OverviewDeviceRowModel;

OverviewModel application_overview_model_new(Application *self,
                                                BluezClient *bluez,
                                                ObexService *obex_service);
void application_overview_model_clear(OverviewModel *model);
gboolean application_overview_model_has_blocked_power_status(const OverviewModel *model);
OverviewDeviceRowModel application_overview_device_row_model_new(Application *self,
                                                                     BluezClient *bluez,
                                                                     const char *device_path);
void application_overview_device_row_model_clear(OverviewDeviceRowModel *model);
