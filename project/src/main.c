#include <stdio.h>
#include "store.h"

void test();

void basicCallback(int status)
{
    printf("status is %d\n", status);
}

void readCallback(int status, int resourceCount, struct resource resources[])
{
    for (int i = 0; i < resourceCount; i++) {
        struct resource pls = resources[i];
        printf("Name: %s, value: %s\n", pls.name, pls.value);
    }
}

int main()
{
    store_init();
    test();
}

void test()
{

}