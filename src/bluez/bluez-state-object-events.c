#include "bluez/bluez-state-object-events.h"

struct _BluezStateObjectEvents {
  BluezObjectCache *cache;
  BluezStateChanged on_changed;
  gpointer user_data;
};

static void
bluez_state_object_events_on_interfaces_added(const char *path,
                                              GVariant *interfaces,
                                              gpointer user_data)
{
  BluezStateObjectEvents *events = user_data;

  bluez_object_cache_cache_interfaces(events->cache, path, interfaces);
  bluez_object_cache_set_error(events->cache, NULL);
  bluez_state_object_events_notify_changed(events);
}

static void
bluez_state_object_events_on_interfaces_removed(const char *path,
                                                GVariant *interfaces,
                                                gpointer user_data)
{
  BluezStateObjectEvents *events = user_data;

  bluez_object_cache_remove_interfaces(events->cache, path, interfaces);
  bluez_state_object_events_notify_changed(events);
}

static void
bluez_state_object_events_on_properties_changed(const char *object_path,
                                                const char *changed_interface,
                                                GVariant *changed,
                                                GVariant *invalidated,
                                                gpointer user_data)
{
  BluezStateObjectEvents *events = user_data;

  bluez_object_cache_apply_properties_changed(events->cache,
                                              object_path,
                                              changed_interface,
                                              changed,
                                              invalidated);
  bluez_state_object_events_notify_changed(events);
}

BluezStateObjectEvents *
bluez_state_object_events_new(BluezObjectCache *cache,
                              BluezStateChanged on_changed,
                              gpointer user_data)
{
  BluezStateObjectEvents *events = g_new0(BluezStateObjectEvents, 1);

  events->cache = cache;
  events->on_changed = on_changed;
  events->user_data = user_data;
  return events;
}

void
bluez_state_object_events_free(BluezStateObjectEvents *events)
{
  g_free(events);
}

void
bluez_state_object_events_start(BluezStateObjectEvents *events,
                                BluezObjectManager *manager)
{
  if (!events || !manager)
    return;

  bluez_object_manager_start(manager,
                             bluez_state_object_events_on_interfaces_added,
                             bluez_state_object_events_on_interfaces_removed,
                             bluez_state_object_events_on_properties_changed,
                             events);
}

void
bluez_state_object_events_notify_changed(BluezStateObjectEvents *events)
{
  if (events && events->on_changed)
    events->on_changed(events->user_data);
}
