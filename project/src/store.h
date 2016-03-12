#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 50
#define MAX_VALUE_LENGTH 50
#define MAX_NUM_OF_ENTRIES 5
#define MAX_RESOURCES 128

/**
 * A key-value resource
 */
struct resource
{
    char name[MAX_NAME_LENGTH];
    char value[MAX_VALUE_LENGTH];
};

void store_init();

/**
 * Lock the requested resources to the node with the specified "id".
 * Only the node with the specified "id" will be able to read/write the data until unlocked again.
 * Other nodes will be unable to access the data.
 *
 * Returns:
 * 0: success
 * 1: lock already exists on at least one of the requested resources
 * 2: out of space to acquire locks
 */
int lock(char* name);

/**
 * Returns 0 if the resource is unlocked, otherwise 1
 */
int is_unlocked(char* name);

/**
 * Unlock the requested resources so that some other node can use them.
 * Will fail if the specified "id" was not the same one used to lock the resources originally
 *
 * Returns:
 * 0: success
 * 1: generic error
 */
int unlock(char* name);

/**
 * Writes the provided list of resources, if a lock exists for all the resources for the specified id.
 *
 * Returns:
 * 0: success
 * 1: lock didn't exist for id and resources
 * 2: ran out of space to put resources
 */
int store_write(int resourceCount, struct resource* entryList);

/**
 * Will read the resources listed in "nameList". If a lock does not exist for all
 * requested resources, or the lock is not for the requesting id, then the resources will not be read.
 * Puts the returned values in entryList, and expects it to already be allocated.
 *
 * For resource which don't exist, they will be returned, but the value will be ""
 *
 * Returns:
 * 0: success
 * 1: generic error
 */
int store_read(int nameCount, char** nameList, struct resource* entryList);

#endif // STORE_H_INCLUDED
