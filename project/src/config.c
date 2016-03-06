//
// Created by mitch on 2/24/16.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "config.h"

int _node_with_address(int, char*, char*, struct node*);

int node_count(char *filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Can't open file at: %s\n", filename);
        return -1;
    }

    int lines = 0, currentPort;
    char currentAddress[IP_ADDRESS_SIZE];

    // Variables just there to only count lines that match structure. They aren't used.
    while (fscanf(fp, "%s %d", currentAddress, &currentPort) != EOF)
    {
        lines++;
    }

    fclose(fp);
    return lines;
}

int all_nodes(char *filename, struct node nodes[])
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Can't open file at: %s\n", filename);
        return -1;
    }

    int currentId = 0, currentPort;
    char currentAddress[IP_ADDRESS_SIZE];

    // Variables just there to only count lines that match structure. They aren't used.
    while (fscanf(fp, "%s %d", currentAddress, &currentPort) != EOF)
    {
        strcpy(nodes[currentId].address, currentAddress);
        nodes[currentId].port = currentPort;
        nodes[currentId].id = currentId;
        currentId++;
    }

    fclose(fp);
    return currentId;
}

int node_for_id(char *filename, char *address, int port)
{
    struct node* currentNode = malloc(sizeof(struct node));
    int lineFound = 0;
    while ((lineFound = _node_with_address(lineFound, filename, address, currentNode)) != -1)
    {
        if (currentNode->port == port) break;
        lineFound++;
    }

    if (lineFound == -1) return -1;
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

    struct node* nodePtr = malloc(sizeof(struct node));
    int lineFound = _node_with_address(0, filename, address, nodePtr);
    if (lineFound == -1)
    {
        free(nodePtr);
        return NULL;
    }
    return nodePtr;
}

/**
 * Returns the line on which the node config was found, otherwise -1
 */
int _node_with_address(int lineOffset, char* filename, char* address, struct node*nodePtr)
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
        if (currentLine < lineOffset) continue;

        if (!strcmp(address, currentAddress))
        {
            fclose(fp);

            nodePtr->id = currentLine;
            nodePtr->port = currentPort;
            strcpy(nodePtr->address, currentAddress);
            return currentLine;
        }
    }

    fclose(fp);
    return -1;
}
