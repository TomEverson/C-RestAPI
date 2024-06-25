#include <microhttpd.h>
#include <string.h>
#include <stdio.h>
#include "routes/route.h"
#include "db/db.h"

#define PORT 8888

static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls)
{
    // Check method and handle routes
    if (strcmp(url, "/") == 0)
    {
        return base_route_handler(connection);
    }
    else if (strcmp(url, "/users") == 0)
    {
        return user_route_handler(connection, method, upload_data);
    }
    else
    {
        return MHD_NO;
    }
}

int main()
{

    // Open SQLite database
    if (!open_database())
    {
        return 1;
    }

    struct MHD_Daemon *daemon;

    // Start the server on localhost and port 8888
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY,
                              8888,             // Port number
                              NULL,             // Accept policy callback (can be NULL)
                              NULL,             // Accept policy callback argument (can be NULL)
                              &request_handler, // Access handler callback
                              NULL,             // Access handler callback argument (can be NULL)
                              MHD_OPTION_END);

    if (!daemon)
    {
        fprintf(stderr, "Error starting the server.\n");
        close_database();
        return 1;
    }

    printf("Server running on port %d...\n", PORT);
    getchar(); // Wait for a key press to exit

    close_database();
    MHD_stop_daemon(daemon);
    return 0;
}
