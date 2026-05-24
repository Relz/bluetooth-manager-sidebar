#pragma once

#include <glib.h>

typedef struct _BluezDiscoveryTimer BluezDiscoveryTimer;
typedef gboolean (*BluezDiscoveryTimeoutCallback)(gpointer user_data);

BluezDiscoveryTimer *bluez_discovery_timer_new(void);
void bluez_discovery_timer_free(BluezDiscoveryTimer *timer);
void bluez_discovery_timer_clear(BluezDiscoveryTimer *timer);
void bluez_discovery_timer_schedule(BluezDiscoveryTimer *timer,
                                    guint seconds,
                                    BluezDiscoveryTimeoutCallback callback,
                                    gpointer user_data,
                                    GDestroyNotify destroy);
