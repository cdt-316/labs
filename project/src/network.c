//
// Created by mitch on 3/6/16.
//

#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include "network.h"
#include "store.h"

struct connection connections[MAX_NODES];

void _connection_listen(int, struct node *);
void _attempt_connections(int, struct node *);
void _parse_request(int, char[]);

void network_init(int nodeCount, struct node* this)
{
    for (int i = 0; i < nodeCount; i++)
    {
        connections[i].node = node_for_id(i);
        connections[i].socket = 0;
    }

    _attempt_connections(nodeCount, this);

    pid_t childPid = fork();
    if (childPid < 0)
        perror("Forking failed\n");

    if (childPid == 0)
    {
        printf("Forked thread to listen for connections\n");
        prctl(PR_SET_PDEATHSIG, SIGHUP); // Ask for forked thread to be killed when parent dies
        _connection_listen(nodeCount, this);
    }

    getchar();
}

void _attempt_connections(int nodeCount, struct node *this)
{
    for (int i = 0; i < nodeCount; i++)
    {
        if (i == this->id)
            continue;

        struct node* current = node_for_id(i);

        int sockd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_port = htons((uint16_t) current->port);
        dest.sin_addr.s_addr = inet_addr(current->address);
        memset(&(dest.sin_zero), '\0', 8);

        if (connect(sockd, (struct sockaddr*)&dest, sizeof(struct sockaddr)))
        {
            close(sockd);
            continue;
        }

        printf("network * Connected to node %d: (%s:%d)\n", i, current->address, current->port);
        connections[i].socket = sockd;
    }
}

void _connection_listen(int nodeCount, struct node *this)
{
    char buffer[1024];
    int listenSocket;
    fd_set fds;

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

            char* ipAddress = inet_ntoa(address.sin_addr);
            int port = ntohs(address.sin_port);

            int nodeId = node_id(ipAddress, port);
            printf("network * Node %d connected: (%s:%d)\n", nodeId, ipAddress, port);
            connections[nodeId].socket = newSocket;
        }

        for (int i = 0; i < nodeCount; i++)
        {
            struct node* node = connections[i].node;
            int socket = connections[i].socket;
            if (FD_ISSET(socket, &fds))
            {
                ssize_t size = read(socket, buffer, 1024);
                if (size == 0)
                {
                    //Close
                    printf("network * Node %d disconnected: (%s:%d)\n", node->id, node->address, node->port);
                    close(socket);
                    connections[i].socket = 0;
                    continue;
                }

                char copy[size];
                strncpy(copy, buffer, (size_t)size - 1);
                copy[size - 1] = '\0';

                printf("network * N%d -> \"%s\"\n", node->id, copy);
                _parse_request(node->id, copy);

                if (strlen(copy) == 0)
                {
                    continue;
                }

                send(socket, copy, strlen(copy), 0);
            }
        }
    }
#pragma clang diagnostic pop
}

void _parse_request(int nodeId, char* message)
{
    char command = message[0];
    int response;
    switch (command)
    {
        case 'L':
            message++; //Remove 'L' from message
            char* name = strtok(message, "\n");

            if (name == NULL){
                strcpy(message, "!1\n");
                break;
            }

            sprintf(message - 1, "!%d\n", is_unlocked(name));
            break;
        case 'W':
            message++; // Remove 'W' from message

            struct resource* list = malloc(sizeof(struct resource) * MAX_RESOURCES);
            char* resourceName = strtok(message, "= ");
            char* resourceValue = strtok(NULL, "= ");
            int i;
            for (i = 0; i < MAX_RESOURCES; i++)
            {

                if (resourceName == NULL || resourceValue == NULL) {
                    break;
                }

                strcpy(list[i].name, resourceName);
                strcpy(list[i].value, resourceValue);
                printf("network * Writing %s to %s\n", list[i].name, list[i].value);

                resourceName = strtok(NULL, "= ");
                resourceValue = strtok(NULL, "= ");
            }

            response = store_write(nodeId, i, list);
            free(list);

            if (response < 0) response = 1;
            sprintf(message - 1, "!%d\n", response);
            break;
        case '!':
            printf("network * Reply to previous request\n");
            strcpy(message, "");
            break;
        default:
            printf("network * Invalid command '%c'\n", command);
            strcpy(message, "");
            break;
    }
}