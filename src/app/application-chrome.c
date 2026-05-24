#include "app/application-chrome.h"

#include "app/application-chrome-state.h"
#include "app/application-window-access.h"
#include "sidebar/sidebar-window.h"

static void
application_chrome_apply_state(Application *self, const ApplicationChromeState *state)
{
  GtkWindow *window = application_window(self);

  if (!window || !state)
    return;

  sidebar_window_set_title(window, state->title);
  sidebar_window_set_overview_mode(window, state->overview);
  sidebar_window_set_power_state(window,
                                 state->power_visible,
                                 state->power_sensitive,
                                 state->power_active);
}

void
application_chrome_refresh(Application *self)
{
  ApplicationChromeState state = application_chrome_state_new(self);

  application_chrome_apply_state(self, &state);
  application_chrome_state_clear(&state);
}
