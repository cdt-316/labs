#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

#define MAX_STRING_LEN  50

/**A database entry*/
struct resource
{
    char name[MAX_STRING_LEN];
    char value[MAX_STRING_LEN];
}

/**Lock resources in the local database, specified by the names in the nameList. Returns 1 on success, or 0 on failure.*/
int lock(int id, char** nameList);

/**Unlock resources in the local database, specified by the names in the nameList. Returns 1 on success, or 0 on failure.*/
int unlock(int id, char** nameList);


int s_write(int id, struct resource* entryList);


struct resource* s_read(int id, char** nameList);

#endif // STORE_H_INCLUDED
