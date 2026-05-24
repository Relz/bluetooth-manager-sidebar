#pragma once

#include <gio/gio.h>

#include "bluez/bluez-cache.h"

typedef struct _BluezState BluezState;

typedef void (*BluezStateChanged)(gpointer user_data);

BluezState *bluez_state_new(BluezStateChanged on_changed,
                                   gpointer user_data,
                                   GDestroyNotify destroy);
void bluez_state_free(BluezState *state);
gboolean bluez_state_connect(BluezState *state);
void bluez_state_shutdown(BluezState *state);
gboolean bluez_state_refresh(BluezState *state);
void bluez_state_refresh_if_active(BluezState *state, gboolean disposed);
/* Borrowed, nullable connection. */
GDBusConnection *bluez_state_connection(BluezState *state);
const char *bluez_state_primary_adapter(BluezState *state);
const char *bluez_state_error(BluezState *state);
/* Borrowed cache. */
BluezObjectCache *bluez_state_cache(BluezState *state);
