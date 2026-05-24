#pragma once

#include "app/application-device-settings-context.h"

void application_device_settings_sections_add_audio(const DeviceSettingsContext *context,
                                                    AdwPreferencesGroup *group);
void application_device_settings_sections_add_file_transfer(const DeviceSettingsContext *context,
                                                            AdwPreferencesGroup *group);
void application_device_settings_sections_add_identity(const DeviceSettingsContext *context,
                                                       AdwPreferencesGroup *group);
void application_device_settings_sections_add_security(const DeviceSettingsContext *context,
                                                       AdwPreferencesGroup *group);
void application_device_settings_sections_add_battery(const DeviceSettingsContext *context,
                                                      AdwPreferencesGroup *group);
void application_device_settings_sections_add_details(const DeviceSettingsContext *context,
                                                      AdwPreferencesGroup *group);
void application_device_settings_sections_add_removal(const DeviceSettingsContext *context,
                                                      AdwPreferencesGroup *group);
