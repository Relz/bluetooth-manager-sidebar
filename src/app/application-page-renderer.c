#include "app/application-pages.h"

#include "app/application-adapter-info-page.h"
#include "app/application-chrome.h"
#include "app/application-device-settings-page.h"
#include "app/application-overview-page.h"
#include "app/application-private.h"
#include "app/application-settings-page.h"
#include "app/application-window-access.h"
#include "sidebar/sidebar-window.h"

#define APPLICATION_SIGNAL_REFRESH_COALESCE_MS 300

static void
application_page_renderer_clear_content(Application *self)
{
  GtkWindow *window = application_window(self);

  if (!window)
    return;

  sidebar_window_clear_content(window);
}

static gboolean
application_has_visible_window(Application *self)
{
  GtkWindow *window = application_window(self);

  return window != NULL && gtk_widget_get_visible(GTK_WIDGET(window));
}

void
application_mark_dirty(Application *self)
{
  g_return_if_fail(IS_APPLICATION(self));

  self->refresh_dirty = TRUE;
}

void
application_refresh_now(Application *self)
{
  GtkWindow *window;
  ApplicationPage current_page;
  gboolean preserve_scroll;
  double scroll_value = 0.0;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_window(self);
  if (!window) {
    application_mark_dirty(self);
    return;
  }

  g_clear_handle_id(&self->refresh_source, g_source_remove);
  g_clear_handle_id(&self->signal_refresh_source, g_source_remove);
  self->refresh_dirty = FALSE;

  current_page = application_pages_current_page(self);
  preserve_scroll = self->has_rendered_page && self->last_rendered_page == current_page;
  if (preserve_scroll)
    scroll_value = sidebar_window_scroll_value(window);

  application_chrome_refresh(self);
  application_page_renderer_clear_content(self);

  switch (current_page) {
    case APPLICATION_PAGE_ADAPTER_INFO:
      application_adapter_info_page_build(self);
      break;
    case APPLICATION_PAGE_SETTINGS:
      application_settings_page_build(self);
      break;
    case APPLICATION_PAGE_DEVICE_SETTINGS:
      application_device_settings_page_build(self);
      break;
    case APPLICATION_PAGE_OVERVIEW:
    default:
      application_overview_page_build(self);
      break;
  }

  self->last_rendered_page = current_page;
  self->has_rendered_page = TRUE;
  if (preserve_scroll)
    sidebar_window_restore_scroll(window, scroll_value);
}

static void
application_run_scheduled_refresh(Application *self, gboolean visible_only)
{
  gboolean visible = application_has_visible_window(self);

  if (visible_only && !visible) {
    application_mark_dirty(self);
    return;
  }
  if (!visible) {
    application_mark_dirty(self);
    return;
  }

  application_refresh_now(self);
}

static gboolean
application_refresh_timeout_cb(gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  gboolean visible_only = self->refresh_visible_only;

  self->refresh_source = 0;
  self->refresh_visible_only = FALSE;
  application_run_scheduled_refresh(self, visible_only);
  return G_SOURCE_REMOVE;
}

static gboolean
application_signal_refresh_timeout_cb(gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  self->signal_refresh_source = 0;
  application_run_scheduled_refresh(self, TRUE);
  return G_SOURCE_REMOVE;
}

void
application_schedule_refresh(Application *self, guint delay_ms, gboolean visible_only)
{
  g_return_if_fail(IS_APPLICATION(self));

  if (!application_has_visible_window(self)) {
    application_mark_dirty(self);
    return;
  }

  if (self->refresh_source != 0) {
    self->refresh_visible_only = self->refresh_visible_only && visible_only;
    return;
  }

  self->refresh_visible_only = visible_only;
  self->refresh_source = g_timeout_add_full(G_PRIORITY_DEFAULT,
                                            delay_ms,
                                            application_refresh_timeout_cb,
                                            g_object_ref(self),
                                            g_object_unref);
}

void
application_schedule_signal_refresh(Application *self)
{
  g_return_if_fail(IS_APPLICATION(self));

  if (!application_has_visible_window(self)) {
    application_mark_dirty(self);
    return;
  }

  if (self->signal_refresh_source != 0)
    return;

  self->signal_refresh_source = g_timeout_add_full(G_PRIORITY_DEFAULT,
                                                   APPLICATION_SIGNAL_REFRESH_COALESCE_MS,
                                                   application_signal_refresh_timeout_cb,
                                                   g_object_ref(self),
                                                   g_object_unref);
}
