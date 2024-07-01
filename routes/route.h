#include "../db/db.h"
#include "../utils/utils.h"
#include <microhttpd.h>

int base_route_handler(struct MHD_Connection *connection);

int user_route_handler(struct MHD_Connection *connection,
                       const char *method,
                       const char *upload_data,
                       size_t *upload_data_size,
                       void **con_cls);