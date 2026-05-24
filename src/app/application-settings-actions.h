#pragma once

#include "app/application.h"

void application_actions_on_auto_accept_changed(GObject *object,
                                                GParamSpec *param,
                                                gpointer user_data);
void application_actions_on_received_location_activated(GtkEntry *entry,
                                                       gpointer user_data);
void application_actions_on_received_location_focus_changed(GObject *object,
                                                           GParamSpec *param,
                                                           gpointer user_data);
