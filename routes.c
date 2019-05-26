#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <errno.h>
#include "php_parser.c"
#define STATUS_OK "200 OK\n"
#define STATUS_NOT_FOUND "404 NOT FOUND\n"

char *get_status(int status);
char *determineContentType(char *url);
void handle_request(char *buffer, int socket);

void handle_request(char *buffer, int socket)
{
    char header_delim[] = "\r\n\r\n";
    char method_delim[] = " ";
    char protocol[] = "HTTP/1.1 ";
    char *payload, *msg, *url, *content_type;
    char *file_location = malloc(128);
    int status = 200;
    FILE *f;
    char not_found[] = "<title>404 Not Found</title>"
                       "<h1>Not Found</h1>"
                       "<p>The requested URL was not found on the server.  If you entered the URL manually please check your spelling and try again.</p>";

    //extract first line in http header
    char *method_url = strtok(buffer, header_delim);

    //extract http method
    char *http_method = strtok(method_url, method_delim);

    //extract url
    char *temp_url = strtok(NULL, method_delim);
    // printf(">>>>>>>>>>>>>> %s\n", temp_url);
    if (strcmp(temp_url, "/") == 0)
    {
        url = "/index.html";
    }
    else
    {
        url = temp_url;
    }
    content_type = determineContentType(url);
    strcat(file_location, "files");
    strcat(file_location, url);
    f = fopen(file_location, "rb");

    //http method switch case
    if (f)
    {
        if (strcmp(http_method, "GET") == 0)
        {
            f = fopen(file_location, "rb");
            if (strcmp(content_type, "Content-Type: text/html\n") == 0)
            {
                payload = php_compile(file_location);
                msg = malloc(sizeof(payload) + 8192);
                fclose(f);
            }
            else if (strcmp(content_type, "Content-Type: image/jpg\n") == 0 ||
                     strcmp(content_type, "Content-Type: image/jpeg\n") == 0 ||
                     strcmp(content_type, "Content-Type: image/png\n") == 0)
            {
                // image handling not working
                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                fseek(f, 0, SEEK_SET);
                msg = malloc(fsize + 8192);
                payload = malloc(fsize);
                fread(payload, 1, fsize, f);
                fclose(f);
            }
            else
            {
                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                fseek(f, 0, SEEK_SET);
                msg = malloc(fsize + 8192);
                payload = malloc(fsize);
                fread(payload, 1, fsize, f);
                fclose(f);
            }
        }
        else if (strcmp(http_method, "POST") == 0)
        {
            // msg = malloc(8192);
            // status = 404;
            // payload = not_found;
            payload = php_post_compile(file_location, buffer);
            msg = malloc(sizeof(payload) + 8192);
            fclose(f);
        }
        else
        {
            msg = malloc(8192);
            status = 404;
            payload = not_found;
        }
    }
    else
    {
        msg = malloc(8192);
        status = 404;
        payload = not_found;
    }
    strcat(msg, protocol);
    strcat(msg, get_status(status));
    strcat(msg, content_type);
    strcat(msg, "\n");
    strcat(msg, payload);
    payload = NULL;
    file_location = NULL;
    free(payload);
    free(file_location);
    if ((write(socket, msg, strlen(msg))) < 0)
    {
        perror("Error when sending message to server");
    }
    msg = NULL;
    free(msg);
    close(socket);
}

char *get_status(int status)
{
    if (status == 200)
    {
        return STATUS_OK;
    }
    else
    {
        return STATUS_NOT_FOUND;
    }
}

char *determineContentType(char *url)
{
    char *dot = strrchr(url, '.');
    if (dot)
    {
        if (strcmp(dot, ".css") == 0)
        {
            return "Content-Type: text/css\n";
        }
        else if (strcmp(dot, ".jpg") == 0)
        {
            return "Content-Type: image/jpg\n";
        }
        else if (strcmp(dot, ".jpeg") == 0)
        {
            return "Content-Type: image/jpeg\n";
        }
        else if (strcmp(dot, ".png") == 0)
        {
            return "Content-Type: image/png\n";
        }
        else
        {
            return "Content-Type: text/html\n";
        }
    }
    return "Content-Type: text/html\n";
}