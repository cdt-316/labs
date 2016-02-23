//
// Created by mitch on 2/23/16.
//

#include "store.h"

void store_init()
{

}

int lock(int id, int nameCount, char* nameList[])
{
    return 0;
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
