#pragma once

#include "bluez/bluez-object-manager.h"

typedef struct {
  BluezObjectManagerInterfacesAdded on_interfaces_added;
  BluezObjectManagerInterfacesRemoved on_interfaces_removed;
  BluezObjectManagerPropertiesChanged on_properties_changed;
  gpointer user_data;
} BluezObjectManagerSignalCallbacks;

void bluez_object_manager_signals_dispatch_interfaces_added(
  const BluezObjectManagerSignalCallbacks *callbacks,
  GVariant *parameters);
void bluez_object_manager_signals_dispatch_interfaces_removed(
  const BluezObjectManagerSignalCallbacks *callbacks,
  GVariant *parameters);
void bluez_object_manager_signals_dispatch_properties_changed(
  const BluezObjectManagerSignalCallbacks *callbacks,
  const char *object_path,
  GVariant *parameters);
