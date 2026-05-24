#pragma once

#include "app/application.h"

GtkWidget *application_page_action_row(const char *title, const char *subtitle);
GtkWidget *application_page_labeled_entry_row(const char *title,
                                              const char *text,
                                              const char *placeholder,
                                              GtkWidget **out_entry);
GtkWidget *application_page_selectable_property_row(const char *title,
                                                    const char *value);
GtkWidget *application_page_switch_row(const char *title,
                                       gboolean active,
                                       gboolean sensitive,
                                       GtkWidget **out_switch);
GtkWidget *application_page_button_row(const char *title,
                                       const char *subtitle,
                                       const char *button_label,
                                       gboolean destructive,
                                       GtkWidget **out_button);
