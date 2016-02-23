#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

#define MAX_NAME_LENGTH 50
#define MAX_VALUE_LENGTH 50

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
 * 1: generic error
 */
int lock(int id, int nameCount, char* nameList[]);

/**
 * Unlock the requested resources so that some other node can use them.
 * Will fail if the specified "id" was not the same one used to lock the resources originally
 *
 * Returns:
 * 0: success
 * 1: generic error
 */
int unlock(int id, int nameCount, char* nameList[]);

/**
 * Writes the provided list of resources, if a lock exists for all the resources for the specified id.
 *
 * Returns:
 * 0: success
 * 1: generic error
 */
int store_write(int id, int resourceCount, struct resource entryList[]);

/**
 * Will read the resources listed in "nameList". If a lock does not exist for all
 * requested resources, or the lock is not for the requesting id, then the resources will not be read.
 *
 * Returns:
 * 0: success
 * 1: generic error
 */
int store_read(int id, int nameCount, char* nameList[], int* entryCount, struct resource entryList[]);

#endif // STORE_H_INCLUDED
