//
// Created by mitch on 2/23/16.
//

#include "store.h"

void store_init()
{

}

void lock(int id, int nameCount, char* nameList[], void callback(int))
{
    callback(0);
}

void unlock(int id, int nameCount, char* nameList[], void callback(int))
{
    callback(0);
}

void store_write(int id, int resourceCount, struct resource entryList[], void callback(int))
{
    callback(0);
}

void store_read(int id, int nameCount, char* nameList[], void callback(int, int, struct resource[]))
{
    struct resource resources[3] = {
            {"Obama", "true"},
            {"Biden", "false"},
    };
    callback(0, 2, resources);
}
