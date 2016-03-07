//
// Created by mitch on 2/24/16.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "config.h"

int nodeCount = -1;
struct node* nodes[16];

int config_init(char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Can't open file at: %s\n", filename);
        return -1;
    }

    int currentLine = 0, currentPort;
    char currentAddress[IP_ADDRESS_SIZE];

    while (fscanf(fp, "%s %d", currentAddress, &currentPort) != EOF)
    {
        struct node* current = malloc(sizeof(struct node));
        current->id = currentLine;
        strcpy(current->address, currentAddress);
        current->port = currentPort;
        nodes[currentLine] = current;
        currentLine++;
    }

    nodeCount = currentLine;
    fclose(fp);
    return -1;
}

int node_count()
{
    return nodeCount;
}

struct node* node_for_id(int id)
{
    return nodes[id];
}

int node_id(char *address, int port)
{
    for (int i = 0; i < nodeCount; i++)
    {
        struct node* current = nodes[i];
        if (!strcmp(address, current->address) && port == current->port)
            return current->id;
    }

    return -1;
}

struct node* this_node()
{
    struct ifaddrs *ifaddr, *ifa;
    char address[IP_ADDRESS_SIZE];

    if (getifaddrs(&ifaddr) == -1)
    {
        printf("Failed to get network interface data\n");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

        // Only want to deal with IPv4, and the "loopback" interface is not the one we want
        if (ifa->ifa_addr->sa_family != AF_INET || !strcmp(ifa->ifa_name, "lo")) {
            continue;
        }

        int code = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                           address, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
        if (code != 0) {
            printf("getnameinfo() failed\n");
            return NULL;
        }
    }
    freeifaddrs(ifaddr);

    for (int i = 0; i < nodeCount; i++)
    {
        if (!(strcmp(address, nodes[i]->address)))
            return nodes[i];
    }

    return NULL;
}
