#pragma once

#include <glib.h>

typedef enum {
  APPLICATION_PAGE_OVERVIEW,
  APPLICATION_PAGE_ADAPTER_INFO,
  APPLICATION_PAGE_SETTINGS,
  APPLICATION_PAGE_DEVICE_SETTINGS,
} ApplicationPage;

typedef struct _ApplicationPageController ApplicationPageController;

ApplicationPageController *application_page_controller_new(void);
void application_page_controller_free(ApplicationPageController *controller);

ApplicationPage application_page_controller_current_page(ApplicationPageController *controller);
const char *application_page_controller_selected_device_path(ApplicationPageController *controller);

void application_page_controller_show_overview(ApplicationPageController *controller);
void application_page_controller_show_adapter_info(ApplicationPageController *controller);
void application_page_controller_show_settings(ApplicationPageController *controller);
void application_page_controller_show_device_settings(ApplicationPageController *controller,
                                                      const char *device_path);
