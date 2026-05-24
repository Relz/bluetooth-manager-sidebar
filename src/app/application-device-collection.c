#include "app/application-device-model.h"

#include "app/application-data-access.h"
#include "app/application-device-presentation.h"
#include "bluez/bluez-client-query.h"

static gint
application_compare_device_names(gconstpointer left, gconstpointer right, gpointer user_data)
{
  Application *self = user_data;
  const char *left_path = *(char *const *)left;
  const char *right_path = *(char *const *)right;
  g_autofree char *left_name = application_device_presentation_dup_display_name(self, left_path);
  g_autofree char *right_name = application_device_presentation_dup_display_name(self, right_path);
  gint result = g_utf8_collate(left_name, right_name);

  if (result != 0)
    return result;

  g_autofree char *left_address = bluez_properties_dup_string(application_device_properties(self, left_path), "Address");
  g_autofree char *right_address = bluez_properties_dup_string(application_device_properties(self, right_path), "Address");

  result = g_utf8_collate(left_address ? left_address : "", right_address ? right_address : "");
  if (result != 0)
    return result;

  return g_strcmp0(left_path, right_path);
}

static gint
application_compare_available_devices(gconstpointer left, gconstpointer right, gpointer user_data)
{
  Application *self = user_data;
  const char *left_path = *(char *const *)left;
  const char *right_path = *(char *const *)right;
  BluezProperties *left_device = application_device_properties(self, left_path);
  BluezProperties *right_device = application_device_properties(self, right_path);
  gboolean left_found = FALSE;
  gboolean right_found = FALSE;
  gint left_rssi = bluez_properties_get_int(left_device, "RSSI", &left_found);
  gint right_rssi = bluez_properties_get_int(right_device, "RSSI", &right_found);

  if (left_found && right_found && left_rssi != right_rssi)
    return right_rssi - left_rssi;
  if (left_found != right_found)
    return left_found ? -1 : 1;

  return application_compare_device_names(left, right, user_data);
}

GPtrArray *
application_device_model_collect_devices(Application *self,
                                         BluezClient *bluez,
                                         const char *adapter_path,
                                         ApplicationDeviceGroup group)
{
  g_auto(GStrv) paths = bluez ? bluez_client_dup_adapter_device_paths(bluez, adapter_path) : NULL;
  GPtrArray *devices = g_ptr_array_new_with_free_func(g_free);

  for (guint i = 0; paths && paths[i]; i++) {
    BluezProperties *device = application_device_properties(self, paths[i]);
    gboolean connected = bluez_properties_get_bool(device, "Connected", FALSE);
    gboolean paired = bluez_properties_get_bool(device, "Paired", FALSE);
    gboolean include = FALSE;

    switch (group) {
      case APPLICATION_DEVICE_GROUP_CONNECTED:
        include = connected;
        break;
      case APPLICATION_DEVICE_GROUP_PAIRED:
        include = paired && !connected;
        break;
      case APPLICATION_DEVICE_GROUP_AVAILABLE:
        include = !paired;
        break;
    }

    if (include)
      g_ptr_array_add(devices, g_strdup(paths[i]));
  }

  if (group == APPLICATION_DEVICE_GROUP_AVAILABLE)
    g_ptr_array_sort_with_data(devices, application_compare_available_devices, self);
  else
    g_ptr_array_sort_with_data(devices, application_compare_device_names, self);

  return devices;
}
