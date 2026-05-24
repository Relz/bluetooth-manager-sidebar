#include "sidebar/sidebar-window-panel.h"

#include "sidebar/sidebar-chrome.h"
#include "core/ui-constants.h"

#define SCROLL_RESTORE_ATTEMPTS 8
#define SCROLL_RESTORE_DELAY_MS 16

static void sidebar_window_panel_update_scroll_fades(SidebarWindowState *state);

static gboolean
sidebar_window_panel_point_inside_panel(SidebarWindowState *state,
                                        GtkGestureClick *gesture,
                                        gdouble x,
                                        gdouble y)
{
  GtkWidget *gesture_widget = NULL;
  graphene_rect_t bounds;

  if (!state || !state->toast_overlay)
    return FALSE;

  gesture_widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
  if (!gesture_widget ||
      !gtk_widget_compute_bounds(GTK_WIDGET(state->toast_overlay), gesture_widget, &bounds))
    return FALSE;

  return x >= bounds.origin.x && x < bounds.origin.x + bounds.size.width &&
         y >= bounds.origin.y && y < bounds.origin.y + bounds.size.height;
}

static gboolean
sidebar_window_panel_on_surface_get_child_position(GtkOverlay *overlay,
                                                   GtkWidget *widget,
                                                   GdkRectangle *allocation,
                                                   gpointer user_data)
{
  SidebarWindowState *state = user_data;
  int surface_width;
  int panel_width;

  if (!state || widget != GTK_WIDGET(state->toast_overlay))
    return FALSE;

  surface_width = MAX(0, gtk_widget_get_width(GTK_WIDGET(overlay)));
  panel_width = MIN(WIDTH, surface_width);

  allocation->x = MAX(0, surface_width - panel_width);
  allocation->y = 0;
  allocation->width = panel_width;
  allocation->height = MAX(0, gtk_widget_get_height(GTK_WIDGET(overlay)));
  return TRUE;
}

static GtkWidget *
sidebar_window_panel_scroll_fade(const char *css_class, GtkAlign valign)
{
  GtkWidget *fade = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_widget_add_css_class(fade, "bm-scroll-fade");
  gtk_widget_add_css_class(fade, css_class);
  gtk_widget_set_can_target(fade, FALSE);
  gtk_widget_set_halign(fade, GTK_ALIGN_FILL);
  gtk_widget_set_valign(fade, valign);
  gtk_widget_set_hexpand(fade, TRUE);
  gtk_widget_set_size_request(fade, -1, 44);
  gtk_widget_set_opacity(fade, 0.0);
  return fade;
}

static GtkAdjustment *
sidebar_window_panel_scroll_adjustment(SidebarWindowState *state)
{
  if (!state || !state->scrolled)
    return NULL;
  return gtk_scrolled_window_get_vadjustment(state->scrolled);
}

static void
sidebar_window_panel_on_scroll_adjustment_value_changed(GtkAdjustment *adjustment, gpointer user_data)
{
  (void)adjustment;
  sidebar_window_panel_update_scroll_fades(user_data);
}

static void
sidebar_window_panel_on_scroll_adjustment_notify(GObject *object,
                                                 GParamSpec *pspec,
                                                 gpointer user_data)
{
  (void)object;
  (void)pspec;
  sidebar_window_panel_update_scroll_fades(user_data);
}

static void
sidebar_window_panel_update_scroll_fades(SidebarWindowState *state)
{
  GtkAdjustment *adjustment = sidebar_window_panel_scroll_adjustment(state);
  double lower;
  double upper;
  double page_size;
  double value;
  double max_value;
  gboolean can_scroll;

  if (!adjustment || !state->scroll_fade_top || !state->scroll_fade_bottom)
    return;

  lower = gtk_adjustment_get_lower(adjustment);
  upper = gtk_adjustment_get_upper(adjustment);
  page_size = gtk_adjustment_get_page_size(adjustment);
  value = gtk_adjustment_get_value(adjustment);
  max_value = MAX(lower, upper - page_size);
  can_scroll = upper - lower > page_size + 0.5;

  gtk_widget_set_opacity(state->scroll_fade_top, can_scroll && value > lower + 0.5 ? 1.0 : 0.0);
  gtk_widget_set_opacity(state->scroll_fade_bottom, can_scroll && value < max_value - 0.5 ? 1.0 : 0.0);
}

static gboolean
sidebar_window_panel_restore_scroll_cb(gpointer user_data)
{
  SidebarWindowState *state = user_data;
  GtkAdjustment *adjustment = sidebar_window_panel_scroll_adjustment(state);
  double lower;
  double upper;
  double page_size;
  double max_value;
  double value;

  if (!adjustment) {
    state->scroll_restore_source = 0;
    state->scroll_restore_attempts = 0;
    return G_SOURCE_REMOVE;
  }

  lower = gtk_adjustment_get_lower(adjustment);
  upper = gtk_adjustment_get_upper(adjustment);
  page_size = gtk_adjustment_get_page_size(adjustment);
  max_value = MAX(lower, upper - page_size);
  value = CLAMP(state->pending_scroll_value, lower, max_value);
  gtk_adjustment_set_value(adjustment, value);
  sidebar_window_panel_update_scroll_fades(state);

  state->scroll_restore_attempts++;
  if (max_value >= state->pending_scroll_value - 0.5 ||
      state->scroll_restore_attempts >= SCROLL_RESTORE_ATTEMPTS) {
    state->scroll_restore_source = 0;
    state->scroll_restore_attempts = 0;
    return G_SOURCE_REMOVE;
  }

  return G_SOURCE_CONTINUE;
}

static void
sidebar_window_panel_on_surface_pressed(GtkGestureClick *gesture,
                                        gint n_press,
                                        gdouble x,
                                        gdouble y,
                                        gpointer user_data)
{
  SidebarWindowState *state = user_data;

  (void)n_press;

  if (state)
    state->outside_click_started = !sidebar_window_panel_point_inside_panel(state, gesture, x, y);
}

static void
sidebar_window_panel_on_surface_released(GtkGestureClick *gesture,
                                         gint n_press,
                                         gdouble x,
                                         gdouble y,
                                         gpointer user_data)
{
  SidebarWindowState *state = user_data;
  gboolean should_release = state && state->outside_click_started &&
                             !sidebar_window_panel_point_inside_panel(state, gesture, x, y);

  if (state)
    state->outside_click_started = FALSE;

  if (should_release && state->surface_released) {
    void (*callback)(GtkGestureClick *, gint, gdouble, gdouble, gpointer) =
      (void (*)(GtkGestureClick *, gint, gdouble, gdouble, gpointer))state->surface_released;
    callback(gesture, n_press, x, y, state->surface_released_user_data);
  }
}

GtkWidget *
sidebar_window_panel_build(const SidebarWindowCallbacks *callbacks,
                           SidebarWindowState *state)
{
  GtkWidget *surface = gtk_overlay_new();
  GtkWidget *click_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *panel_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *scroll_overlay = gtk_overlay_new();
  GtkWidget *scroll = gtk_scrolled_window_new();
  GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  GtkGesture *surface_click = gtk_gesture_click_new();
  GtkWidget *chrome_widget;
  GtkWidget *panel;
  GtkAdjustment *adjustment;

  gtk_widget_add_css_class(surface, "bm-sidebar-surface");
  gtk_widget_set_hexpand(surface, TRUE);
  gtk_widget_set_vexpand(surface, TRUE);
  gtk_overlay_set_child(GTK_OVERLAY(surface), click_area);
  g_signal_connect(surface,
                   "get-child-position",
                   G_CALLBACK(sidebar_window_panel_on_surface_get_child_position),
                   state);
  gtk_widget_add_controller(click_area, GTK_EVENT_CONTROLLER(surface_click));
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(surface_click), 0);
  g_signal_connect(surface_click, "pressed", G_CALLBACK(sidebar_window_panel_on_surface_pressed), state);
  g_signal_connect(surface_click, "released", G_CALLBACK(sidebar_window_panel_on_surface_released), state);
  if (callbacks && callbacks->surface_released) {
    state->surface_released = callbacks->surface_released;
    state->surface_released_user_data = callbacks->user_data;
  }

  state->toast_overlay = ADW_TOAST_OVERLAY(adw_toast_overlay_new());
  panel = GTK_WIDGET(state->toast_overlay);
  gtk_widget_add_css_class(panel, "bm-sidebar-panel");
  gtk_widget_set_halign(panel, GTK_ALIGN_END);
  gtk_widget_set_valign(panel, GTK_ALIGN_FILL);
  gtk_widget_set_vexpand(panel, TRUE);
  gtk_widget_set_size_request(panel, WIDTH, -1);
  gtk_overlay_add_overlay(GTK_OVERLAY(surface), panel);

  gtk_widget_set_margin_top(panel_content, 12);
  gtk_widget_set_margin_bottom(panel_content, 12);
  gtk_widget_set_margin_start(panel_content, 12);
  gtk_widget_set_margin_end(panel_content, 12);
  adw_toast_overlay_set_child(state->toast_overlay, panel_content);

  state->chrome = sidebar_chrome_new(callbacks ? &callbacks->chrome : NULL);
  chrome_widget = sidebar_chrome_widget(state->chrome);
  if (chrome_widget)
    gtk_box_append(GTK_BOX(panel_content), chrome_widget);

  state->content_box = GTK_BOX(content_box);
  state->scrolled = GTK_SCROLLED_WINDOW(scroll);
  gtk_widget_set_margin_bottom(content_box, 6);
  gtk_widget_set_vexpand(scroll_overlay, TRUE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), content_box);
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_overlay_set_child(GTK_OVERLAY(scroll_overlay), scroll);

  state->scroll_fade_top = sidebar_window_panel_scroll_fade("bm-scroll-fade-top", GTK_ALIGN_START);
  state->scroll_fade_bottom = sidebar_window_panel_scroll_fade("bm-scroll-fade-bottom", GTK_ALIGN_END);
  gtk_overlay_add_overlay(GTK_OVERLAY(scroll_overlay), state->scroll_fade_top);
  gtk_overlay_add_overlay(GTK_OVERLAY(scroll_overlay), state->scroll_fade_bottom);

  adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
  g_signal_connect(adjustment, "value-changed", G_CALLBACK(sidebar_window_panel_on_scroll_adjustment_value_changed), state);
  g_signal_connect(adjustment, "notify::lower", G_CALLBACK(sidebar_window_panel_on_scroll_adjustment_notify), state);
  g_signal_connect(adjustment, "notify::upper", G_CALLBACK(sidebar_window_panel_on_scroll_adjustment_notify), state);
  g_signal_connect(adjustment, "notify::page-size", G_CALLBACK(sidebar_window_panel_on_scroll_adjustment_notify), state);

  gtk_box_append(GTK_BOX(panel_content), scroll_overlay);

  return surface;
}

void
sidebar_window_panel_clear_content(SidebarWindowState *state)
{
  GtkWidget *child;

  if (!state || !state->content_box)
    return;

  while ((child = gtk_widget_get_first_child(GTK_WIDGET(state->content_box))))
    gtk_box_remove(state->content_box, child);
}

void
sidebar_window_panel_append_content(SidebarWindowState *state, GtkWidget *child)
{
  if (state && state->content_box && child)
    gtk_box_append(state->content_box, child);
}

double
sidebar_window_panel_scroll_value(SidebarWindowState *state)
{
  GtkAdjustment *adjustment = sidebar_window_panel_scroll_adjustment(state);

  if (!adjustment)
    return 0.0;
  if (state->scroll_restore_source)
    return state->pending_scroll_value;
  return gtk_adjustment_get_value(adjustment);
}

void
sidebar_window_panel_restore_scroll(SidebarWindowState *state, double value)
{
  if (!state || !state->scrolled)
    return;

  state->pending_scroll_value = value;
  state->scroll_restore_attempts = 0;
  if (!state->scroll_restore_source)
    state->scroll_restore_source = g_timeout_add(SCROLL_RESTORE_DELAY_MS,
                                                sidebar_window_panel_restore_scroll_cb,
                                                state);
}
