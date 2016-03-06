//
// Created by mitch on 3/6/16.
//

#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "network.h"
#include "config.h"

struct connection* connections;

void network_init(int nodeCount, struct node* this)
{
    connections = malloc(sizeof(struct connection) * nodeCount);
    int listenSocket;
    fd_set fds;

    for (int i = 0; i < nodeCount; i++)
    {
        connections[i].node = node_for_id(i);
        connections[i].socket = 0;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
        perror("socket() on listenSocket failed\n");

    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)))
        perror("Making listenSocket reusable failed\n");

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)this->port);
    address.sin_addr.s_addr = inet_addr(this->address);
    memset(&(address.sin_zero), '\0', 8);

    if (bind(listenSocket, (struct sockaddr *)&address, sizeof(struct sockaddr)))
        perror("Failed to bind listenSocket\n");

    if (listen(listenSocket, 5))
        perror("Failed to listen on listenSocket\n");

    printf("network * Listening for connections\n");
    int addrLength = sizeof(address);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(listenSocket, &fds);
        int maxDescriptor = listenSocket;

        for (int i = 0; i < nodeCount; i++)
        {
            int client = connections[i].socket;
            if (client)
                FD_SET(client, &fds);

            if (client > maxDescriptor)
                maxDescriptor = client;
        }

        int activity = select(maxDescriptor + 1, &fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR)
            perror("Failed to select()\n");

        if (FD_ISSET(listenSocket, &fds))
        {
            // New connection
            int newSocket = accept(listenSocket, (struct sockaddr*)&address, (socklen_t*)&addrLength);
            if (newSocket < 0)
                perror("Accept failed\n");

            printf("network * New connection from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        }

    }
#pragma clang diagnostic pop

    /*for (int i = 0; i < nodeCount; i++)
    {
        if (i == this->id) continue;

        struct sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_port = htons((uint16_t)nodes[i].port);
        dest.sin_addr.s_addr = inet_addr(nodes[i].address);
        memset(&(dest.sin_zero), '\0', 8);

        printf("Connecting to %s\n", nodes[i].address);
        if (connect(sockd, (struct sockaddr*)&dest, sizeof(struct sockaddr)))
        {
            perror("Connection failed\n");
        }

        char* message = "pls respond\n";
        ssize_t sent = send(sockd, message, strlen(message), 0);
        printf("Sent %zu bytes\n", sent);
    }*/
}