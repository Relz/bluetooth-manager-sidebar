#include "app/application-settings-runtime.h"

struct _ApplicationSettingsRuntime {
  SettingsStore *store;
  SidebarSettings *settings;
};

ApplicationSettingsRuntime *
application_settings_runtime_new(void)
{
  ApplicationSettingsRuntime *runtime = g_new0(ApplicationSettingsRuntime, 1);

  runtime->store = settings_store_new(NULL);
  runtime->settings = settings_store_load(runtime->store);
  return runtime;
}

void
application_settings_runtime_free(ApplicationSettingsRuntime *runtime)
{
  if (!runtime)
    return;

  g_clear_pointer(&runtime->settings, sidebar_settings_free);
  g_clear_pointer(&runtime->store, settings_store_free);
  g_free(runtime);
}

SettingsStore *
application_settings_runtime_store(ApplicationSettingsRuntime *runtime)
{
  return runtime ? runtime->store : NULL;
}

SidebarSettings *
application_settings_runtime_settings(ApplicationSettingsRuntime *runtime)
{
  return runtime ? runtime->settings : NULL;
}
