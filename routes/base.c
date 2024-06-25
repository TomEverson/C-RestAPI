#include "route.h"
#include "../db/db.h"
#include "../utils/utils.h"
#include <microhttpd.h>

int base_route_handler(struct MHD_Connection *connection)
{
    const char *filename = "view/index.html";
    char *page = read_file(filename);
    struct MHD_Response *mhd_response = MHD_create_response_from_buffer(strlen(page),
                                                                        (void *)page,
                                                                        MHD_RESPMEM_MUST_COPY);

    MHD_add_response_header(mhd_response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);
    return ret;
}
