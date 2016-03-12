#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED
#include "store.h"
#define DBFILE "local_database_file.txt"

/**Writes entries to local database, specified by the entryList. Returns 0 on success, or 1 on failure. */
int db_write(struct resource* entryList, int numOfEntries);

/**Reads entries from local database, specified by the names in the nameList. Returns 0 on success, else 1*/
int db_read(char** nameList, int nameCount, struct resource* entryList);

/**Returns the line were the entry occurs, else -1*/
int entryExists(char* name);

/**Updates the entry in the db-file*/
void updateEntry(char* name);

/**Puts the lin in the db-file pointed to by line, into a db-entry, and returns it on success. Else the entry will be empty.*/
struct resource readEntry(int line);

#endif // DATABASE_H_INCLUDED
