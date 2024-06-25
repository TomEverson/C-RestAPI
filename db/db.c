#include <stdio.h>
#include <sqlite3.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>

// SQLite database connection
sqlite3 *db;

// Open database function
int open_database()
{
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("Database opened successfully\n");
    return 1;
}

// Close database function
void close_database()
{
    sqlite3_close(db);
    printf("Database closed\n");
}

int execute_sql_query(struct MHD_Connection *connection, const char *sql)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return MHD_NO;
    }

    // JSON response construction
    char *json_response = strdup("[");
    int num_cols = sqlite3_column_count(stmt);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        char row[1024] = "{";
        for (int i = 0; i < num_cols; i++)
        {
            const char *col_name = sqlite3_column_name(stmt, i);
            int col_type = sqlite3_column_type(stmt, i);
            char col_value[256];

            switch (col_type)
            {
            case SQLITE_INTEGER:
                snprintf(col_value, sizeof(col_value), "\"%s\":%d", col_name, sqlite3_column_int(stmt, i));
                break;
            case SQLITE_FLOAT:
                snprintf(col_value, sizeof(col_value), "\"%s\":%f", col_name, sqlite3_column_double(stmt, i));
                break;
            case SQLITE_TEXT:
                snprintf(col_value, sizeof(col_value), "\"%s\":\"%s\"", col_name, sqlite3_column_text(stmt, i));
                break;
            case SQLITE_NULL:
                snprintf(col_value, sizeof(col_value), "\"%s\":null", col_name);
                break;
            default:
                snprintf(col_value, sizeof(col_value), "\"%s\":\"\"", col_name); // Default case
                break;
            }

            strcat(row, col_value);
            if (i < num_cols - 1)
            {
                strcat(row, ",");
            }
        }
        strcat(row, "},");

        json_response = realloc(json_response, strlen(json_response) + strlen(row) + 1);
        strcat(json_response, row);
    }

    sqlite3_finalize(stmt); // Finalize the statement

    // Remove trailing comma and close JSON array
    size_t len = strlen(json_response);
    if (json_response[len - 1] == ',')
    {
        json_response[len - 1] = '\0';
    }
    strcat(json_response, "]");

    // Create HTTP response
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json_response), json_response, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return (rc == SQLITE_DONE) ? MHD_YES : MHD_NO;
}