#include <sqlite3.h>
#include <microhttpd.h>

int open_database();

void close_database();

int execute_sql_select_query(struct MHD_Connection *connection, const char *sql);

int execute_sql_insert_query(struct MHD_Connection *connection, const char *sql);