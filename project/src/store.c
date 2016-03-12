//
// Created by mitch on 2/23/16.
//

#define MAX_LOCKS 8
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "store.h"
#include "network.h"

char* locks[MAX_LOCKS];
struct resource* resources[MAX_RESOURCES];

void store_init() {
    for (int i = 0; i < MAX_LOCKS; i++) {
        locks[i] = NULL;
    }

    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i] = NULL;
    }
}

int is_unlocked(char* name)
{
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        if (locks[i] == NULL) continue;
        if (!strcmp(name, locks[i]))
            return 1;
    }

    return 0;
}

int lock(char* name)
{
    int empty = -1;
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        if (locks[i] == NULL)
        {
            empty = i;
            continue;
        }

        if (!strcmp(locks[i], name))
        {
            // Can't allocate lock because one already exists for the requested resource
            return 1;
        }
    }

    // Everything looks good locally, time to check the network
    if (isRemotelyLocked(name))
        return 1;

    // Check if there's space for the new lock
    if (empty == -1)
    {
        return 2;
    }

    locks[empty] = strdup(name);
    return 0;
}

int unlock(char* name)
{
    int index;
    for (index = 0; index < MAX_LOCKS; index++)
    {
        if (locks[index] == NULL) continue;
        if (!strcmp(locks[index], name))
            break;
    }

    if (index == -1) return 1;

    free(locks[index]);
    locks[index] = NULL;
    return 0;
}

int store_write(int resourceCount, struct resource* entryList, int thisOnly)
{
    int needsPosition = 0; // The next resource in the list that needs a location in `resources` to have a pointer
    int resourceIndices[resourceCount]; // A list of "spots" in `resources` for a pointer to a new resource
    char* nameList[resourceCount];

    for (int i = 0; i < resourceCount; i++)
    {
        resourceIndices[i] = -1;
        nameList[i] = entryList[i].name;
    }

    for (int i = 0; i < MAX_RESOURCES; i++)
    {
        if (resources[i] == NULL)
        {
            if (needsPosition < resourceCount) resourceIndices[needsPosition++] = i;
            continue;
        }

        for (int j = 0; j < resourceCount; j++)
        {
            // If this is one of the resources that we're writing to
            if (!strcmp(resources[i]->name, nameList[j]))
            {
                // This resource already exists. If we had a "spot" in `resources` to put this one, lets donate it
                // to the next resource that needs it
                if (needsPosition < resourceCount) {
                    resourceIndices[needsPosition++] = resourceIndices[j];
                    resourceIndices[j] = -1;
                }
                strncpy(resources[i]->value, entryList[j].value, MAX_VALUE_LENGTH);
            }
        }
    }

    db_write(entryList, resourceCount);

    if (needsPosition < resourceCount)
    {
        return 2;
    }

    for (int i = 0; i < resourceCount; i++)
    {
        int index = resourceIndices[i];
        if (index == -1) continue;

        struct resource* newResource = malloc(sizeof(struct resource));
        strncpy(newResource->name, nameList[i], MAX_NAME_LENGTH);
        strncpy(newResource->value, entryList[i].value, MAX_VALUE_LENGTH);
        resources[index] = newResource;
    }

    if (thisOnly)
        return 0;

    return remote_write(resourceCount, entryList);
}

int store_read(int nameCount, char** nameList, struct resource* entryList)
{
    for (int i = 0; i < nameCount; i++)
    {
        // Some resources won't exist, and they should be returned with the proper name and an empty string as the value
        strncpy(entryList[i].name, nameList[i], MAX_NAME_LENGTH);
        strcpy(entryList[i].value, "");
    }

    for (int i = 0; i < MAX_RESOURCES; i++)
    {
        if (resources[i] == NULL) continue;

        for (int j = 0; j < nameCount; j++)
        {
            // If this is one of the resources that we want
            if (!strcmp(resources[i]->name, nameList[j]))
            {
                strncpy(entryList[j].value, resources[i]->value, MAX_VALUE_LENGTH);
            }
        }
    }

    return 0;
}
