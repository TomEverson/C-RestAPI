#include "route.h"
#include "../db/db.h"
#include "../utils/utils.h"
#include <microhttpd.h>
#include <cjson/cJSON.h>

struct ConnectionInfo
{
    int post_done;
    char *post_data;
    size_t post_data_size;
};

static int send_response(struct MHD_Connection *connection, const char *response_str, unsigned int status_code)
{
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_str), (void *)response_str, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

int user_route_handler(struct MHD_Connection *connection,
                       const char *method,
                       const char *upload_data,
                       size_t *upload_data_size,
                       void **con_cls)
{

    if (NULL == *con_cls)
    {
        struct ConnectionInfo *con_info;

        con_info = malloc(sizeof(struct ConnectionInfo));
        if (NULL == con_info)
        {
            return MHD_NO;
        }

        con_info->post_done = 0;
        con_info->post_data = NULL;
        con_info->post_data_size = 0;

        *con_cls = (void *)con_info;

        return MHD_YES;
    }

    struct ConnectionInfo *con_info = *con_cls;

    if (strcmp(method, "GET") == 0)
    {
        const char *select_sql = "SELECT * FROM Users";
        return execute_sql_select_query(connection, select_sql);
    }
    else if (strcmp(method, "POST") == 0)
    {
        //! Construct The JSON Data
        if (*upload_data_size != 0)
        {
            con_info->post_data = realloc(con_info->post_data, con_info->post_data_size + *upload_data_size + 1);
            memcpy(con_info->post_data + con_info->post_data_size, upload_data, *upload_data_size);
            con_info->post_data_size += *upload_data_size;
            con_info->post_data[con_info->post_data_size] = '\0';
            *upload_data_size = 0;
            return MHD_YES;
        }
        else if (!con_info->post_done)
        {
            con_info->post_done = 1;

            // Parse JSON data
            cJSON *json = cJSON_Parse(con_info->post_data);
            if (!json)
            {
                const char *error_response = "Invalid JSON";
                return send_response(connection, error_response, MHD_HTTP_BAD_REQUEST);
            }

            const cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
            const cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");

            if (!cJSON_IsString(id) || !cJSON_IsString(name))
            {
                const char *error_response = "Missing 'id' or 'name' in JSON data";
                cJSON_Delete(json);
                return send_response(connection, error_response, MHD_HTTP_BAD_REQUEST);
            }

            // Print the id and name
            printf("Received POST data - ID: %s, Name: %s\n", id->valuestring, name->valuestring);

            char insert_sql[256];
            snprintf(insert_sql, sizeof(insert_sql), "INSERT INTO Users (id, name) VALUES ('%s', '%s')", id->valuestring, name->valuestring);
            cJSON_Delete(json);
            return execute_sql_insert_query(connection, insert_sql);
        }
    }
}