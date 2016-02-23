#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED
#include "store.h"

/**Writes entries to local database, specified by the entryList. Returns 1 on success, or 0 on failure. */
int db_write(struct resource* entryList);

/**Reads entries from local database, specified by the names in the nameList. Returns a list of all the matched entries.*/
struct resource* db_read(char** nameList);

#endif // DATABASE_H_INCLUDED
