#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "routes.c"

#define PORT 8080
#define BUFFER_SIZE 30000
int main()
{
    int server_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error when creating socket");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(server_socket, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("Error when binding socket");
        return 1;
    }

    if (listen(server_socket, 3) < 0)
    {
        perror("Error during listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int new_socket;
        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Error when accepting connection");
            exit(EXIT_FAILURE);
        }
        char buffer[BUFFER_SIZE] = {0};
        // handle_request(buffer, new_socket);
        // printf("Request complete\n");
        int pid = fork();
        if (pid == -1)
        {
            perror("Error when forking");
            // break;
        }
        else if (pid > 0)
        {
            // printf("Request handled\n");
            close(new_socket);
        }
        else if (pid == 0)
        {
            close(server_socket);
            valread = read(new_socket, buffer, BUFFER_SIZE);
            if (valread < 0)
            {
                perror("No bytes to read");
            }
            handle_request(buffer, new_socket);
            break;
        }
    }
    return 0;
}