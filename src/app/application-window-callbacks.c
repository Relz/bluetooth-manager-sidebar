#include "app/application-window-callbacks.h"

#include <gdk/gdkkeysyms.h>

#include "app/application-adapter-actions.h"
#include "app/application-pages.h"

static void
on_surface_released(GtkGestureClick *gesture,
                    gint n_press,
                    gdouble x,
                    gdouble y,
                    gpointer user_data)
{
  (void)gesture;
  (void)n_press;
  (void)x;
  (void)y;
  application_hide_sidebar(APPLICATION(user_data));
}

static gboolean
on_window_close_request(GtkWindow *window, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)window;
  application_hide_sidebar(self);
  return TRUE;
}

static gboolean
on_key_pressed(GtkEventControllerKey *controller,
               guint keyval,
               guint keycode,
               GdkModifierType state,
               gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)controller;
  (void)keycode;
  (void)state;

  if (keyval == GDK_KEY_Escape) {
    if (application_pages_current_page(self) != APPLICATION_PAGE_OVERVIEW)
      application_pages_show_overview(self);
    else
      application_hide_sidebar(self);
    return TRUE;
  }

  return FALSE;
}

static void
on_back_clicked(GtkButton *button, gpointer user_data)
{
  (void)button;
  application_pages_show_overview(APPLICATION(user_data));
}

static void
on_info_clicked(GtkButton *button, gpointer user_data)
{
  (void)button;
  application_pages_show_adapter_info(APPLICATION(user_data));
}

static void
on_settings_clicked(GtkButton *button, gpointer user_data)
{
  (void)button;
  application_pages_show_settings(APPLICATION(user_data));
}

SidebarWindowCallbacks
application_window_callbacks(Application *self)
{
  return (SidebarWindowCallbacks){
    .user_data = self,
    .surface_released = G_CALLBACK(on_surface_released),
    .key_pressed = G_CALLBACK(on_key_pressed),
    .close_request = G_CALLBACK(on_window_close_request),
    .chrome = {
      .user_data = self,
      .back_clicked = G_CALLBACK(on_back_clicked),
      .info_clicked = G_CALLBACK(on_info_clicked),
      .settings_clicked = G_CALLBACK(on_settings_clicked),
      .adapter_power_changed = G_CALLBACK(application_actions_on_adapter_power_changed),
    },
  };
}
