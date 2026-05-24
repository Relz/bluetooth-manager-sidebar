#include "sidebar/sidebar-chrome.h"

struct _SidebarChrome {
  GtkWidget *topbar;
  GtkLabel *page_title_label;
  GtkButton *back_button;
  GtkButton *info_button;
  GtkButton *settings_button;
  GtkSwitch *power_switch;
  gulong power_switch_handler_id;
};

SidebarChrome *
sidebar_chrome_new(const SidebarChromeCallbacks *callbacks)
{
  SidebarChrome *chrome = g_new0(SidebarChrome, 1);
  GtkWidget *title = gtk_label_new(NULL);

  chrome->topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  chrome->back_button = GTK_BUTTON(gtk_button_new_from_icon_name("go-previous-symbolic"));
  chrome->info_button = GTK_BUTTON(gtk_button_new_from_icon_name("dialog-information-symbolic"));
  chrome->settings_button = GTK_BUTTON(gtk_button_new_from_icon_name("preferences-system-symbolic"));
  chrome->power_switch = GTK_SWITCH(gtk_switch_new());

  gtk_widget_set_margin_bottom(chrome->topbar, 12);
  gtk_widget_add_css_class(GTK_WIDGET(chrome->back_button), "flat");
  gtk_widget_add_css_class(GTK_WIDGET(chrome->info_button), "flat");
  gtk_widget_add_css_class(GTK_WIDGET(chrome->settings_button), "flat");
  gtk_widget_set_tooltip_text(GTK_WIDGET(chrome->back_button), "Back");
  gtk_widget_set_tooltip_text(GTK_WIDGET(chrome->info_button), "Adapter Information");
  gtk_widget_set_tooltip_text(GTK_WIDGET(chrome->settings_button), "Settings");
  gtk_widget_set_tooltip_text(GTK_WIDGET(chrome->power_switch), "Adapter Power");
  gtk_widget_set_valign(GTK_WIDGET(chrome->power_switch), GTK_ALIGN_CENTER);

  if (callbacks && callbacks->back_clicked)
    g_signal_connect(chrome->back_button, "clicked", callbacks->back_clicked, callbacks->user_data);
  if (callbacks && callbacks->info_clicked)
    g_signal_connect(chrome->info_button, "clicked", callbacks->info_clicked, callbacks->user_data);
  if (callbacks && callbacks->settings_clicked)
    g_signal_connect(chrome->settings_button, "clicked", callbacks->settings_clicked, callbacks->user_data);
  if (callbacks && callbacks->adapter_power_changed) {
    chrome->power_switch_handler_id = g_signal_connect(chrome->power_switch,
                                                       "notify::active",
                                                       callbacks->adapter_power_changed,
                                                       callbacks->user_data);
  }

  chrome->page_title_label = GTK_LABEL(title);
  gtk_widget_add_css_class(title, "title-2");
  gtk_label_set_xalign(chrome->page_title_label, 0.0f);
  gtk_label_set_ellipsize(chrome->page_title_label, PANGO_ELLIPSIZE_END);
  gtk_widget_set_hexpand(title, TRUE);

  gtk_box_append(GTK_BOX(chrome->topbar), GTK_WIDGET(chrome->back_button));
  gtk_box_append(GTK_BOX(chrome->topbar), title);
  gtk_box_append(GTK_BOX(chrome->topbar), GTK_WIDGET(chrome->info_button));
  gtk_box_append(GTK_BOX(chrome->topbar), GTK_WIDGET(chrome->settings_button));
  gtk_box_append(GTK_BOX(chrome->topbar), GTK_WIDGET(chrome->power_switch));

  return chrome;
}

void
sidebar_chrome_free(SidebarChrome *chrome)
{
  g_free(chrome);
}

GtkWidget *
sidebar_chrome_widget(SidebarChrome *chrome)
{
  return chrome ? chrome->topbar : NULL;
}

void
sidebar_chrome_set_title(SidebarChrome *chrome, const char *title)
{
  if (chrome && chrome->page_title_label)
    gtk_label_set_label(chrome->page_title_label, title ? title : "");
}

void
sidebar_chrome_set_overview_mode(SidebarChrome *chrome, gboolean overview)
{
  if (!chrome)
    return;

  gtk_widget_set_visible(GTK_WIDGET(chrome->back_button), !overview);
  gtk_widget_set_visible(GTK_WIDGET(chrome->info_button), overview);
  gtk_widget_set_visible(GTK_WIDGET(chrome->settings_button), overview);
}

void
sidebar_chrome_set_power_state(SidebarChrome *chrome,
                               gboolean visible,
                               gboolean sensitive,
                               gboolean active)
{
  if (!chrome || !chrome->power_switch)
    return;

  gtk_widget_set_visible(GTK_WIDGET(chrome->power_switch), visible);
  gtk_widget_set_sensitive(GTK_WIDGET(chrome->power_switch), sensitive);
  if (chrome->power_switch_handler_id)
    g_signal_handler_block(chrome->power_switch, chrome->power_switch_handler_id);
  gtk_switch_set_active(chrome->power_switch, active);
  if (chrome->power_switch_handler_id)
    g_signal_handler_unblock(chrome->power_switch, chrome->power_switch_handler_id);
}
