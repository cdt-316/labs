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

int store_read(int id, int nameCount, char* nameList[], int* entryCount, struct resource resources[])
{
    *entryCount = 2;
    resources[0] = {"Obama", "true"};
    resources[1] = {"Biden", "false"};
    return 0;
}
