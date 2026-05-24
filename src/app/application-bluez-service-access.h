#pragma once

#include "app/application.h"
#include "bluez/bluez-client.h"

BluezClient *application_services_bluez_client(Application *self);
