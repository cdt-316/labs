#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED
#include "store.h"
#define DBFILE "local_database_file.txt"

/**Writes entries to local database, specified by the entryList. Returns 1 on success, or 0 on failure. */
int db_write(struct resource* entryList, int numOfEntries);

/**Reads entries from local database, specified by the names in the nameList. Returns 1 on success, else 0*/
int db_read(char** nameList, int nameCount, struct resource* entryList);

int entryExists(char* name);

void updateEntry(char* name);

char* readName(int line);

char* readValue(int line);


#endif // DATABASE_H_INCLUDED
