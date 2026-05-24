#pragma once

#include "app/application.h"

void application_actions_on_device_activate(GtkWidget *widget, gpointer user_data);
void application_actions_on_trust_device_clicked(GtkButton *button, gpointer user_data);
void application_actions_on_device_settings_clicked(GtkButton *button, gpointer user_data);
void application_actions_on_alias_entry_activated(GtkEntry *entry, gpointer user_data);
void application_actions_on_alias_entry_focus_changed(GObject *object,
                                                     GParamSpec *param,
                                                     gpointer user_data);
void application_actions_on_device_trusted_changed(GObject *object,
                                                   GParamSpec *param,
                                                   gpointer user_data);
void application_actions_on_device_blocked_changed(GObject *object,
                                                   GParamSpec *param,
                                                   gpointer user_data);
void application_actions_on_remove_device_clicked(GtkButton *button, gpointer user_data);
