//
// Created by mitch on 3/6/16.
//

#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "network.h"

#define LISTEN_PORT 9001

int responses[MAX_NODES];
struct connection connections[MAX_NODES];
struct node* this;
sem_t remoteLockSemaphore;
int timedOut;

void * _connection_listen(void *);
void _attempt_connections(int, struct node *);
void _parse_request(char[], int);

void network_init(int nodeCount, struct node* thisNode)
{
    this = thisNode;
    for (int i = 0; i < nodeCount; i++)
    {
        connections[i].node = node_for_id(i);
        connections[i].socket = 0;
    }

    _attempt_connections(nodeCount, this);
    sem_init(&remoteLockSemaphore, 0, 0);
    pthread_t thread;
    if (pthread_create(&thread, NULL, _connection_listen, &nodeCount))
        printf("Failed to create listen thread\n");

    usleep(100); // Cheesy delay so that "Enter commands:" appears after "Listening" message
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
        dest.sin_port = htons(LISTEN_PORT);
        dest.sin_addr.s_addr = inet_addr(current->address);
        memset(&(dest.sin_zero), '\0', 8);

        if (connect(sockd, (struct sockaddr*)&dest, sizeof(struct sockaddr)))
        {
            close(sockd);
            continue;
        }

        printf("network * Connected to node: %d|%s\n", i, current->address);
        connections[i].socket = sockd;
    }
}

//void _connection_listen(int nodeCount, struct node *this)
void * _connection_listen(void * arg)
{
    int nodeCount = *(int *)(arg);
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
    address.sin_port = htons(LISTEN_PORT);
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

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(maxDescriptor + 1, &fds, NULL, NULL, &timeout);

        if (activity < 0 && errno != EINTR)
            perror("Failed to select()\n");

        if (activity == 0)
        {
            timedOut++;
            if (timedOut == 2)
            {
                sem_post(&remoteLockSemaphore);
            }
            continue;
        }

        if (FD_ISSET(listenSocket, &fds))
        {
            // New connection
            int newSocket = accept(listenSocket, (struct sockaddr*)&address, (socklen_t*)&addrLength);
            if (newSocket < 0)
                perror("Accept failed\n");

            char* ipAddress = inet_ntoa(address.sin_addr);

            int nodeId = node_id(ipAddress);
            printf("network * Node connected: %d|%s\n", nodeId, ipAddress);
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
                    printf("network * Node disconnected: %d|%s\n", node->id, node->address);
                    close(socket);
                    connections[i].socket = 0;
                    continue;
                }

                char copy[size];
                strncpy(copy, buffer, (size_t)size - 1);
                copy[size - 1] = '\0';

                //printf("network * N%d -> \"%s\"\n", node->id, copy);
                _parse_request(copy, node->id);

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

void _parse_request(char* message, int id)
{
    char command = message[0];
    int response;
    char responseNumber[2];
    switch (command)
    {
        case 'L':
            printf("network * Node requested lock: %d\n", id);
            message++; //Remove 'L' from message
            char* name = strtok(message, "\n");

            if (name == NULL){
                strcpy(message, "!1\n");
                break;
            }

            sprintf(message - 1, "!%d\n", is_unlocked(name));
            break;
        case 'W':
            printf("network * Node requested write: %d\n", id);
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

                resourceName = strtok(NULL, "= ");
                resourceValue = strtok(NULL, "= ");
            }

            response = store_write(i, list, 1);
            free(list);

            if (response < 0) response = 1;
            sprintf(message - 1, "!%d\n", response);
            break;
        case '!':
            responseNumber[0] = message[1];
            responseNumber[1] = '\0';
            int before = responses[id];
            responses[id] = atoi(responseNumber);

            if (before == 2)
                sem_post(&remoteLockSemaphore);
            strcpy(message, "");
            break;
        default:
            printf("network * Node made invalid request: %d|%c\n", id, command);
            strcpy(message, "");
            break;
    }
}

int isRemotelyLocked(char* resource)
{
    char message[strlen(resource) + 3];
    sprintf(message, "L%s\n", resource);
    for (int i = 0; i < node_count(); i++)
    {
        if (i == this->id)
        {
            responses[i] = 0;
            continue;
        }

        // 0: ACK
        // 1: NACK
        // 2: No response

        responses[i] = 2;
        send(connections[i].socket, message, sizeof(message), 0);
    }

    timedOut = 0;
    int done = 2;
    while (done == 2 && !timedOut)
    {
        sem_wait(&remoteLockSemaphore);
        done = 0;

        for (int i = 0; i < node_count(); i++)
        {
            if (responses[i] > done)
                done = responses[i];
        }
    }

    done = 0;
    if (timedOut)
    {
        for (int i = 0; i < node_count(); i++)
        {
            if (responses[i] > done && responses[i] != 2)
                done = responses[i];
        }
    }
    return done;
}

int remote_write(int resourceCount, struct resource* entryList)
{
    int messageSize = resourceCount * MAX_NAME_LENGTH;
    messageSize += resourceCount * MAX_VALUE_LENGTH;
    messageSize += resourceCount * 2; // for the '=' and ' '
    messageSize += 3; //For W, \n and \0
    char message[messageSize];
    message[0] = 'W';

    int offset = 1;
    for (int i = 0; i < resourceCount; i++)
    {
        // Resource name
        strcpy(message + offset, entryList[i].name);
        offset += strlen(entryList[i].name); // -1 so that the '\0' doesn't remain

        // '=' separator
        message[offset++] = '=';

        // Resource value
        strcpy(message + offset, entryList[i].value);
        offset += strlen(entryList[i].value);

        // ' ' after each name-value tuple
        message[offset++] = ' ';
    }

    message[offset++] = '\n';
    message[offset] = '\0';

    for (int i = 0; i < node_count(); i++)
    {
        if (i == this->id)
        {
            responses[i] = 0;
            continue;
        }

        // 0: ACK
        // 1: NACK
        // 2: No response

        responses[i] = 2;
        send(connections[i].socket, message, sizeof(message), 0);
    }

    timedOut = 0;
    int done = 2;
    while (done == 2 && !timedOut)
    {
        sem_wait(&remoteLockSemaphore);
        done = 0;

        for (int i = 0; i < node_count(); i++)
        {
            if (responses[i] > done)
                done = responses[i];
        }
    }

    done = 0;
    if (timedOut)
    {
        for (int i = 0; i < node_count(); i++)
        {
            if (responses[i] > done && responses[i] != 2)
                done = responses[i];
        }
    }
    return done;
}
