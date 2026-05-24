#pragma once

#include "app/application.h"

typedef struct {
  char *title;
  gboolean overview;
  gboolean power_visible;
  gboolean power_sensitive;
  gboolean power_active;
} ApplicationChromeState;

ApplicationChromeState application_chrome_state_new(Application *self);
void application_chrome_state_clear(ApplicationChromeState *state);
