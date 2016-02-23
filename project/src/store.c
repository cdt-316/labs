//
// Created by mitch on 2/23/16.
//

#define MAX_LOCKS 8
#define MAX_RESOURCES 1024
#include "store.h"

struct lock {
    int id;
    char* names[MAX_NAME_LENGTH];
};

int itemCount;
int lockCount;

struct lock locks[MAX_LOCKS];
struct resource resources[MAX_RESOURCES];

void store_init()
{
    itemCount = lockCount = 0;
}

int lock(int id, int nameCount, char* nameList[])
{

}

int unlock(int id, int nameCount, char* nameList[])
{
    return 0;
}

int store_write(int id, int resourceCount, struct resource entryList[])
{
    return 0;
}

int store_read(int id, int nameCount, char* nameList[], struct resource entryList[])
{
    struct resource r = {"Obama", "true"};
    entryList[0] = r;
    //entryList[1] = {"Biden", "false"};
    return 0;
}
