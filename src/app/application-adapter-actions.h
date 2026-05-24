#pragma once

#include "app/application.h"

void application_actions_on_adapter_power_changed(GObject *object,
                                                  GParamSpec *param,
                                                  gpointer user_data);
void application_actions_on_discoverable_changed(GObject *object,
                                                  GParamSpec *param,
                                                  gpointer user_data);
void application_actions_on_scan_clicked(GtkButton *button, gpointer user_data);
