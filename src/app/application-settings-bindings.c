#include "app/application-settings-bindings.h"

#include "app/application-page-utils.h"
#include "app/application-settings-actions.h"

GtkWidget *
application_settings_bindings_auto_accept_row(Application *self,
                                              gboolean active,
                                              gboolean sensitive)
{
  GtkWidget *toggle = NULL;
  GtkWidget *row = application_page_switch_row("Auto accept files from trusted devices",
                                               active,
                                               sensitive,
                                               &toggle);

  g_signal_connect(toggle, "notify::active", G_CALLBACK(application_actions_on_auto_accept_changed), self);
  return row;
}

GtkWidget *
application_settings_bindings_received_location_row(Application *self, const char *location)
{
  GtkWidget *location_entry = NULL;
  GtkWidget *location_row = application_page_labeled_entry_row("Received files location",
                                                               location,
                                                               NULL,
                                                               &location_entry);

  g_signal_connect(location_entry,
                   "activate",
                   G_CALLBACK(application_actions_on_received_location_activated),
                   self);
  g_signal_connect(location_entry,
                   "notify::has-focus",
                   G_CALLBACK(application_actions_on_received_location_focus_changed),
                   self);
  return location_row;
}
