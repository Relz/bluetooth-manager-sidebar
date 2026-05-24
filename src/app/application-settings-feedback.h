#pragma once

#include "app/application.h"

void application_settings_feedback_save_error(Application *self, const GError *error);
void application_settings_feedback_refresh_if_changed(Application *self, gboolean changed);
