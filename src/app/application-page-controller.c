#include "app/application-page-controller.h"

struct _ApplicationPageController {
  ApplicationPage page;
  char *selected_device_path;
};

ApplicationPageController *
application_page_controller_new(void)
{
  ApplicationPageController *controller = g_new0(ApplicationPageController, 1);

  controller->page = APPLICATION_PAGE_OVERVIEW;
  return controller;
}

void
application_page_controller_free(ApplicationPageController *controller)
{
  if (!controller)
    return;

  g_free(controller->selected_device_path);
  g_free(controller);
}

ApplicationPage
application_page_controller_current_page(ApplicationPageController *controller)
{
  return controller ? controller->page : APPLICATION_PAGE_OVERVIEW;
}

const char *
application_page_controller_selected_device_path(ApplicationPageController *controller)
{
  return controller ? controller->selected_device_path : NULL;
}

void
application_page_controller_show_overview(ApplicationPageController *controller)
{
  if (!controller)
    return;

  controller->page = APPLICATION_PAGE_OVERVIEW;
  g_clear_pointer(&controller->selected_device_path, g_free);
}

void
application_page_controller_show_adapter_info(ApplicationPageController *controller)
{
  if (!controller)
    return;

  controller->page = APPLICATION_PAGE_ADAPTER_INFO;
  g_clear_pointer(&controller->selected_device_path, g_free);
}

void
application_page_controller_show_settings(ApplicationPageController *controller)
{
  if (!controller)
    return;

  controller->page = APPLICATION_PAGE_SETTINGS;
  g_clear_pointer(&controller->selected_device_path, g_free);
}

void
application_page_controller_show_device_settings(ApplicationPageController *controller,
                                                 const char *device_path)
{
  if (!controller || !device_path)
    return;

  controller->page = APPLICATION_PAGE_DEVICE_SETTINGS;
  g_free(controller->selected_device_path);
  controller->selected_device_path = g_strdup(device_path);
}
