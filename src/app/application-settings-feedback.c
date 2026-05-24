#include "app/application-settings-feedback.h"

#include "app/application-pages.h"

void
application_settings_feedback_save_error(Application *self, const GError *error)
{
  application_show_toast(self, error ? error->message : "Unable to save settings");
}

void
application_settings_feedback_refresh_if_changed(Application *self, gboolean changed)
{
  if (changed)
    application_refresh_now(self);
}
