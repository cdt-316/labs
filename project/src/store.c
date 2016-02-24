//
// Created by mitch on 2/23/16.
//

#define MAX_LOCKS 8
#define MAX_RESOURCES 128
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "store.h"

int _lock_exists(int, int, char*[]);

struct lock {
    int id;
    int nameCount;
    char* names[MAX_RESOURCES];
};

struct lock* locks[MAX_LOCKS];
struct resource* resources[MAX_RESOURCES];

void store_init()
{
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        locks[i] = NULL;
    }

    for (int i = 0; i < MAX_RESOURCES; i++)
    {
        resources[i] = NULL;
    }
}

int lock(int id, int nameCount, char* nameList[])
{
    int empty = -1;
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        if (locks[i] == NULL)
        {
            empty = i;
            continue;
        }

        struct lock oldLock = *locks[i];

        for (int oldIndex = 0; oldIndex < oldLock.nameCount; oldIndex++)
        {
            for (int newIndex = 0; newIndex < nameCount; newIndex++)
            {
                // If a lock already exists for a requested resource
                if (!strcmp(oldLock.names[oldIndex], nameList[newIndex]))
                {
                    // Can't allocate lock because one already exists for one of the requested resources
                    return 1;
                }
            }
        }
    }

    if (empty == -1)
    {
        return 2;
    }

    struct lock* newLock = malloc(sizeof(struct lock));
    newLock->id = id;
    newLock->nameCount = nameCount;

    for (int i = 0; i < nameCount; i++)
    {
        newLock->names[i] = nameList[i];
    }

    locks[empty] = newLock;
    return 0;
}

int unlock(int id, int nameCount, char* nameList[])
{
    int index = _lock_exists(id, nameCount, nameList);
    if (index == -1) return 1;

    free(locks[index]);
    locks[index] = NULL;
    return 0;
}

int store_write(int id, int resourceCount, struct resource entryList[])
{
    int needsPosition = 0; // The next resource in the list that needs a location in `resources` to have a pointer
    int resourceIndices[resourceCount]; // A list of "spots" in `resources` for a pointer to a new resource
    char* nameList[resourceCount];

    for (int i = 0; i < resourceCount; i++)
    {
        resourceIndices[i] = -1;
        nameList[i] = entryList[i].name;
    }

    if (_lock_exists(id, resourceCount, nameList) == -1) return 1;

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

    return 0;
}

int store_read(int id, int nameCount, char* nameList[], struct resource entryList[])
{
    if (_lock_exists(id, nameCount, nameList) == -1) return 1;

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

/**
 * Returns the index of the lock that matches the provided resource names and lock owner.
 * If such a lock exists, returns the index of the lock. Otherwise, returns -1.
 */
int _lock_exists(int id, int nameCount, char* nameList[])
{
    int index = -1;
    for (int i = 0; i < MAX_LOCKS; i++)
    {
        if (locks[i] == NULL) continue;
        struct lock existing = *locks[i];
        if (existing.nameCount != nameCount) continue;
        index = i; // This might be the lock to remove

        for (int nameIndex = 0; nameIndex < existing.nameCount; nameIndex++)
        {
            // If any of the names don't match, then this isn't the lock that we're looking for
            if (strcmp(existing.names[nameIndex], nameList[nameIndex]))
            {
                // This is not the lock to remove
                index = -1;
                break;
            }
        }

        if (index != -1) break;
    }

    if (index == -1 || locks[index]->id != id) return -1;
    return index;
}