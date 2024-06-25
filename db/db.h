#include <sqlite3.h>
#include <microhttpd.h>

int open_database();

void close_database();

int execute_sql_query(struct MHD_Connection *connection, const char *sql);