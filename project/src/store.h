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
 * Data provided to callback:
 *  "status":
 * 0: success
 * 1: generic error
 */
void lock(int id, int nameCount, char* nameList[], void callback(int status));

/**
 * Unlock the requested resources so that some other node can use them.
 * Will fail if the specified "id" was not the same one used to lock the resources originally
 *
 * Data provided to callback:
 *  "status":
 * 0: success
 * 1: generic error
 */
void unlock(int id, int nameCount, char* nameList[], void callback(int status));

/**
 * Writes the provided list of resources, if a lock exists for all the resources for the specified id.
 *
 * Data provided to callback:
 *  "status":
 * 0: success
 * 1: generic error
 */
void store_write(int id, int resourceCount, struct resource entryList[], void callback(int status));

/**
 * Will read the resources listed in "nameList". If a lock does not exist for all
 * requested resources, or the lock is not for the requesting id, then the resources will not be read.
 *
 * Data provided to callback:
 *  "status":
 * 0: success
 * 1: generic error
 *  "entryCount"
 * Number of resources returned
 *  "entryList"
 * List of each resource requested
 */
void store_read(int id, int nameCount, char* nameList[],
                void callback(int status, int entryCount, struct resource entryList[]));

#endif // STORE_H_INCLUDED
