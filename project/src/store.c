//
// Created by mitch on 2/23/16.
//

#define MAX_LOCKS 8
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "store.h"
#include "network.h"
#include "database.h"

char* locks[MAX_LOCKS];

void store_init() {
    for (int i = 0; i < MAX_LOCKS; i++) {
        locks[i] = NULL;
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
    if (db_write(entryList, resourceCount))
    {
        return 1;
    }

    if (thisOnly)
        return 0;

    return remote_write(resourceCount, entryList);
}

int store_read(int nameCount, char** nameList, struct resource* entryList)
{
        for(int i = 0; i < nameCount; i++)
        {
            strcpy(entryList[i].name, "";
            strcpy(entryList[i].value, "" );
        }

    return db_read(nameList, nameCount, entryList);
}

int store_add_var(int count, char** dest, char** src, int thisOnly)
{
    struct resource entryList[2];

    for (int i = 0; i < count; i++)
    {
        store_read(1, &dest[i], entryList);
        store_read(1, &src[i], &entryList[1]);
        int intDest = atoi(entryList[0].value);
        intDest += atoi(entryList[1].value);
        snprintf(entryList[0].value, MAX_VALUE_LENGTH, "%d", intDest);
        store_write(1, entryList, false);
    }

    if (!thisOnly) {
        remote_write(1, entryList);
    }

    return 0;
}

int store_add_const(int count, char** dest, int* val, int thisOnly)
{
    struct resource entryList[1];

    for (int i = 0; i < count; i++)
    {
        store_read(1, &dest[i], entryList);
        int intDest = atoi(entryList[0].value) + val[i];
        snprintf(entryList[0].value, MAX_VALUE_LENGTH, "%d", intDest);
        store_write(1, entryList, false);
    }

    if (!thisOnly) {
        remote_write(1, entryList);
    }

    return 0;
}
