#pragma once

#include "app/application.h"
#include "app/application-page-controller.h"

void application_refresh_now(Application *self);
void application_mark_dirty(Application *self);
void application_schedule_refresh(Application *self, guint delay_ms, gboolean visible_only);
void application_schedule_signal_refresh(Application *self);
void application_pages_refresh(Application *self);
void application_pages_show_overview(Application *self);
void application_pages_show_device_settings(Application *self, const char *device_path);
void application_pages_show_adapter_info(Application *self);
void application_pages_show_settings(Application *self);
ApplicationPage application_pages_current_page(Application *self);
gboolean application_pages_is_device_settings(Application *self);
const char *application_pages_selected_device_path(Application *self);
