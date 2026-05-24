#include "bluez/bluez-state.h"

#include "bluez/bluez-object-manager.h"
#include "bluez/bluez-state-errors.h"
#include "bluez/bluez-state-object-events.h"

struct _BluezState {
  BluezObjectManager *manager;
  BluezObjectCache *cache;
  BluezStateObjectEvents *events;
  gpointer user_data;
  GDestroyNotify destroy;
};

BluezState *
bluez_state_new(BluezStateChanged on_changed,
                    gpointer user_data,
                    GDestroyNotify destroy)
{
  BluezState *state = g_new0(BluezState, 1);

  state->cache = bluez_object_cache_new();
  state->events = bluez_state_object_events_new(state->cache, on_changed, user_data);
  state->user_data = user_data;
  state->destroy = destroy;

  return state;
}

void
bluez_state_free(BluezState *state)
{
  if (!state)
    return;

  bluez_state_shutdown(state);
  g_clear_pointer(&state->events, bluez_state_object_events_free);
  g_clear_pointer(&state->cache, bluez_object_cache_free);
  if (state->destroy && state->user_data)
    state->destroy(state->user_data);
  g_free(state);
}

gboolean
bluez_state_connect(BluezState *state)
{
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail(state != NULL, FALSE);

  bluez_state_shutdown(state);
  state->manager = bluez_object_manager_new_system(&error);
  if (!state->manager) {
    bluez_object_cache_clear(state->cache);
    bluez_state_errors_set_system_bus_unavailable(state->cache, error ? error->message : NULL);
    bluez_state_object_events_notify_changed(state->events);
    return FALSE;
  }

  bluez_state_object_events_start(state->events, state->manager);
  bluez_state_refresh(state);
  return TRUE;
}

void
bluez_state_shutdown(BluezState *state)
{
  if (!state)
    return;

  g_clear_pointer(&state->manager, bluez_object_manager_free);
}

gboolean
bluez_state_refresh(BluezState *state)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) managed_objects = NULL;

  g_return_val_if_fail(state != NULL, FALSE);

  if (!state->manager)
    return FALSE;

  managed_objects = bluez_object_manager_get_managed_objects(state->manager, &error);
  if (!managed_objects) {
    bluez_object_cache_clear(state->cache);
    bluez_state_errors_set_bluez_unavailable(state->cache, error ? error->message : NULL);
    bluez_state_object_events_notify_changed(state->events);
    return FALSE;
  }

  bluez_object_cache_replace_from_managed_objects(state->cache, managed_objects);
  bluez_state_object_events_notify_changed(state->events);
  return TRUE;
}

void
bluez_state_refresh_if_active(BluezState *state, gboolean disposed)
{
  if (!disposed && state)
    bluez_state_refresh(state);
}

GDBusConnection *
bluez_state_connection(BluezState *state)
{
  if (!state || !state->manager)
    return NULL;

  return bluez_object_manager_connection(state->manager);
}

const char *
bluez_state_primary_adapter(BluezState *state)
{
  return state ? bluez_object_cache_primary_adapter(state->cache) : NULL;
}

const char *
bluez_state_error(BluezState *state)
{
  return state ? bluez_object_cache_error(state->cache) : NULL;
}

BluezObjectCache *
bluez_state_cache(BluezState *state)
{
  return state ? state->cache : NULL;
}
