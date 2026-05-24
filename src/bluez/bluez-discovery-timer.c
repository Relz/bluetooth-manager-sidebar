#include "bluez/bluez-discovery-timer.h"

struct _BluezDiscoveryTimer {
  guint source_id;
};

typedef struct {
  BluezDiscoveryTimer *timer;
  BluezDiscoveryTimeoutCallback callback;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezDiscoveryTimerSource;

static void
bluez_discovery_timer_source_free(BluezDiscoveryTimerSource *source)
{
  if (!source)
    return;

  if (source->destroy && source->user_data)
    source->destroy(source->user_data);
  g_free(source);
}

static gboolean
bluez_discovery_timer_on_timeout(gpointer user_data)
{
  BluezDiscoveryTimerSource *source = user_data;

  if (source->timer)
    source->timer->source_id = 0;
  return source->callback ? source->callback(source->user_data) : G_SOURCE_REMOVE;
}

BluezDiscoveryTimer *
bluez_discovery_timer_new(void)
{
  return g_new0(BluezDiscoveryTimer, 1);
}

void
bluez_discovery_timer_free(BluezDiscoveryTimer *timer)
{
  if (!timer)
    return;

  bluez_discovery_timer_clear(timer);
  g_free(timer);
}

void
bluez_discovery_timer_clear(BluezDiscoveryTimer *timer)
{
  guint source_id = 0;

  if (!timer || timer->source_id == 0)
    return;

  source_id = timer->source_id;
  timer->source_id = 0;
  g_source_remove(source_id);
}

void
bluez_discovery_timer_schedule(BluezDiscoveryTimer *timer,
                               guint seconds,
                               BluezDiscoveryTimeoutCallback callback,
                               gpointer user_data,
                               GDestroyNotify destroy)
{
  BluezDiscoveryTimerSource *source = NULL;

  if (!timer)
    return;

  bluez_discovery_timer_clear(timer);
  source = g_new0(BluezDiscoveryTimerSource, 1);
  source->timer = timer;
  source->callback = callback;
  source->user_data = user_data;
  source->destroy = destroy;
  timer->source_id = g_timeout_add_seconds_full(G_PRIORITY_DEFAULT,
                                                seconds,
                                                bluez_discovery_timer_on_timeout,
                                                source,
                                                (GDestroyNotify)bluez_discovery_timer_source_free);
}
