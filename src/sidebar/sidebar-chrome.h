#pragma once

#include <adwaita.h>

typedef struct _SidebarChrome SidebarChrome;

typedef struct {
  gpointer user_data;
  GCallback back_clicked;
  GCallback info_clicked;
  GCallback settings_clicked;
  GCallback adapter_power_changed;
} SidebarChromeCallbacks;

SidebarChrome *sidebar_chrome_new(const SidebarChromeCallbacks *callbacks);
void sidebar_chrome_free(SidebarChrome *chrome);
GtkWidget *sidebar_chrome_widget(SidebarChrome *chrome);
void sidebar_chrome_set_title(SidebarChrome *chrome, const char *title);
void sidebar_chrome_set_overview_mode(SidebarChrome *chrome, gboolean overview);
void sidebar_chrome_set_power_state(SidebarChrome *chrome,
                                    gboolean visible,
                                    gboolean sensitive,
                                    gboolean active);
