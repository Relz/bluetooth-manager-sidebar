#pragma once

#include "obex/obex-outgoing-session.h"

gboolean obex_outgoing_transfer_registration_store_if_pending(ObexOutgoingRequest *request,
                                                              const char *transfer_path,
                                                              GVariant *properties,
                                                              char **out_file_name);
