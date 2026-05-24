#include "app/application-obex-status-section.h"

#include "app/application-obex-service-access.h"
#include "app/application-page-status.h"
#include "app/application-page-utils.h"
#include "obex/obex-service-status.h"

void
application_obex_status_section_add(Application *self, AdwPreferencesGroup *group)
{
  ObexService *obex_service = application_services_obex_service(self);
  const char *message = obex_service ? obex_service_status_message(obex_service) : NULL;

  if (application_page_string_has_text(message))
    adw_preferences_group_add(group, application_page_action_row("File receiving", message));
}
