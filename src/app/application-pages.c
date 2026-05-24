#include "app/application-pages.h"

#include "app/application-audio-page-loader.h"
#include "app/application-private.h"

void
application_pages_refresh(Application *self)
{
  application_refresh_now(self);
}

void
application_pages_show_overview(Application *self)
{
  application_page_controller_show_overview(self->page_controller);
  application_pages_refresh(self);
}

void
application_pages_show_device_settings(Application *self, const char *device_path)
{
  if (!device_path)
    return;

  application_page_controller_show_device_settings(self->page_controller, device_path);
  application_audio_page_loader_load_selected_device(self);
  application_pages_refresh(self);
}

void
application_pages_show_adapter_info(Application *self)
{
  application_page_controller_show_adapter_info(self->page_controller);
  application_pages_refresh(self);
}

void
application_pages_show_settings(Application *self)
{
  application_page_controller_show_settings(self->page_controller);
  application_pages_refresh(self);
}

ApplicationPage
application_pages_current_page(Application *self)
{
  return self ? application_page_controller_current_page(self->page_controller) : APPLICATION_PAGE_OVERVIEW;
}

gboolean
application_pages_is_device_settings(Application *self)
{
  return application_pages_current_page(self) == APPLICATION_PAGE_DEVICE_SETTINGS;
}

const char *
application_pages_selected_device_path(Application *self)
{
  return application_page_controller_selected_device_path(self->page_controller);
}
