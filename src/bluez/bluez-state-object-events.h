#pragma once

#include "bluez/bluez-object-manager.h"
#include "bluez/bluez-state.h"

typedef struct _BluezStateObjectEvents BluezStateObjectEvents;

BluezStateObjectEvents *bluez_state_object_events_new(BluezObjectCache *cache,
                                                         BluezStateChanged on_changed,
                                                         gpointer user_data);
void bluez_state_object_events_free(BluezStateObjectEvents *events);
void bluez_state_object_events_start(BluezStateObjectEvents *events,
                                     BluezObjectManager *manager);
void bluez_state_object_events_notify_changed(BluezStateObjectEvents *events);
