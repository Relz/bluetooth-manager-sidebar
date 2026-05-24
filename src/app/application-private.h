#pragma once

#include "app/application.h"

#include "app/application-data-access.h"
#include "app/application-page-controller.h"
#include "app/application-services.h"
#include "app/application-window-access.h"
#include "bluez/bluez-cache.h"

struct _Application {
  AdwApplication parent_instance;

  gboolean start_hidden;
  gboolean unsupported_layer_shell;
  gboolean runtime_failure;
  char *target_output_name;

  ApplicationServices *services;
  GtkWindow *window;
  ApplicationPageController *page_controller;
  ApplicationPage last_rendered_page;
  gboolean has_rendered_page;
  guint refresh_source;
  guint signal_refresh_source;
  gboolean refresh_visible_only;
  gboolean refresh_dirty;
};
