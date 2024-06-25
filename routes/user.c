#include "route.h"
#include "../db/db.h"
#include "../utils/utils.h"
#include <microhttpd.h>

int user_route_handler(
    struct MHD_Connection *connection,
    const char *method,
    const char *upload_data)
{
    if (strcmp(method, "GET") == 0)
    {
        char *select_sql = "SELECT * FROM Users";
        return execute_sql_query(connection, select_sql);
    }
    else if (strcmp(method, "POST") == 0)
    {
        printf("%s", upload_data);
        char *select_sql = "SELECT * FROM Users";
        return execute_sql_query(connection, select_sql);
    }
}
