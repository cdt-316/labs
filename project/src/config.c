//
// Created by mitch on 2/24/16.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "config.h"

struct node* _node_with_address(int, char*, char*);

int id(char* filename, char* address, int port)
{
    struct node* currentNode;
    while ((currentNode = _node_with_address(0, filename, address)) != NULL) //TODO test this line
    {
        if (currentNode->port == port)
        {
            break;
        }

        free(currentNode);
    }

    if (currentNode == NULL) return -1;
    int id = currentNode->id;
    free(currentNode);
    return id;
}

struct node* this_node(char* filename)
{
    struct ifaddrs *ifaddr, *ifa;
    int code, n;
    char address[IP_ADDRESS_SIZE];

    if (getifaddrs(&ifaddr) == -1)
    {
        printf("Failed to get network interface data\n");
        return NULL;
    }

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

        // Only want to deal with IPv4, and the "loopback" interface is not the one we want
        if (ifa->ifa_addr->sa_family != AF_INET || !strcmp(ifa->ifa_name, "lo")) {
            continue;
        }

        code = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                           address, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
        if (code != 0) {
            printf("getnameinfo() failed\n");
            return NULL;
        }
    }

    freeifaddrs(ifaddr);
    return _node_with_address(0, filename, address);
}

struct node* _node_with_address(int lineOffset, char* filename, char* address)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Can't open file at: %s", filename);
        return NULL;
    }

    int currentLine = 0, currentId, currentPort;
    char currentAddress[IP_ADDRESS_SIZE];

    while (fscanf(fp, "%d %s %d", &currentId, currentAddress, &currentPort) != EOF)
    {
        if (currentLine < lineOffset) continue;

        if (!strcmp(address, currentAddress))
        {
            fclose(fp);

            struct node* newNode = (struct node*)malloc(sizeof(struct node));
            newNode->id = currentId;
            newNode->port = currentPort;
            strcpy(newNode->address, currentAddress);
            return newNode;
        }
    }

    fclose(fp);
    return NULL;
}
