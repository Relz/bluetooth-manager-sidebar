#include "app/application-overview-presenter.h"

#include "app/application-overview-actions.h"
#include "app/application-overview-presentation.h"
#include "app/application-overview-view.h"
#include "app/application-page-status.h"
#include "app/application-window-access.h"

static GtkWidget *
application_overview_presenter_device_row(Application *self,
                                          BluezClient *bluez,
                                          const char *device_path)
{
  OverviewDeviceRowModel model = application_overview_device_row_model_new(self,
                                                                              bluez,
                                                                              device_path);
  GtkWidget *row = application_overview_view_device_row(&model);

  application_overview_actions_bind_device_row(self, row, device_path, &model);

  application_overview_device_row_model_clear(&model);
  return row;
}

static AdwPreferencesGroup *
application_overview_presenter_append_device_group(Application *self,
                                                  BluezClient *bluez,
                                                  const char *title,
                                                  GPtrArray *devices)
{
  GtkWidget *group = application_overview_view_device_group(title);

  for (guint i = 0; i < devices->len; i++)
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group),
                              application_overview_presenter_device_row(self,
                                                                        bluez,
                                                                        g_ptr_array_index(devices, i)));
  application_append_content(self, group);
  return ADW_PREFERENCES_GROUP(group);
}

static GtkWidget *
application_overview_presenter_discoverable_row(Application *self,
                                                gboolean discoverable,
                                                gboolean powered)
{
  GtkWidget *toggle = NULL;
  GtkWidget *row = application_overview_view_discoverable_row(discoverable, powered, &toggle);

  application_overview_actions_bind_discoverable_toggle(self, toggle);
  return row;
}

static GtkWidget *
application_overview_presenter_scan_button(Application *self,
                                           gboolean discovering)
{
  GtkWidget *button = application_overview_view_scan_button(discovering);

  application_overview_actions_bind_scan_button(self, button);
  return button;
}

static void
application_overview_presenter_add_device_groups(Application *self,
                                                 BluezClient *bluez,
                                                 const OverviewModel *model)
{
  AdwPreferencesGroup *available_group = NULL;

  if (model->connected_devices->len > 0)
    application_overview_presenter_append_device_group(
      self,
      bluez,
      application_overview_presentation_connected_group_title(),
      model->connected_devices);
  application_overview_presenter_append_device_group(self,
                                                     bluez,
                                                     application_overview_presentation_paired_group_title(),
                                                     model->paired_devices);
  available_group = application_overview_presenter_append_device_group(
    self,
    bluez,
    application_overview_presentation_available_group_title(),
    model->available_devices);
  adw_preferences_group_set_header_suffix(
    available_group,
    application_overview_presenter_scan_button(self, model->discovering));
  if (model->available_devices->len == 0)
    application_overview_view_add_available_empty_row(available_group,
                                                      model->powered,
                                                      model->discovering);
}

void
application_overview_presenter_render(Application *self,
                                      ApplicationOverviewPageModel *page_model)
{
  OverviewModel *model = &page_model->model;
  GtkWidget *status_group = application_overview_view_status_group();

  application_append_content(self, status_group);

  if (!model->adapter_path || !model->adapter) {
    g_autofree char *message = application_page_dup_status_message(self);

    application_overview_view_add_no_adapter_row(ADW_PREFERENCES_GROUP(status_group), message);
    application_overview_view_add_status_rows(&model->status, ADW_PREFERENCES_GROUP(status_group));
    return;
  }

  adw_preferences_group_add(ADW_PREFERENCES_GROUP(status_group),
                            application_overview_presenter_discoverable_row(self,
                                                                            model->discoverable,
                                                                            model->powered));
  application_overview_view_add_blocked_power_row(ADW_PREFERENCES_GROUP(status_group), model);
  application_overview_view_add_status_rows(&model->status, ADW_PREFERENCES_GROUP(status_group));

  application_overview_presenter_add_device_groups(self, page_model->bluez, model);
}
