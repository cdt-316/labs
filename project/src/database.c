#include "database.h"
#include <string.h>
int entryExists(char* name)
{
    FILE* fp = NULL;
    int lineToDelete = 0;
    int line_num = 0;
    char buffer[MAX_NAME_LENGTH];

    //open the original file, and get the line number of the existing entry name
    fp = fopen(DBFILE, "r");

    if(fp == NULL)
    {
        return -1;
    }

    while(fgets(buffer, MAX_NAME_LENGTH, fp) != NULL)
    {
		if( ( strstr(buffer, name) ) != NULL )
		{
            lineToDelete = line_num;
            fclose(fp);
            return lineToDelete;
        }

        line_num++;
    }

     fclose(fp);
    return -1;
}

void updateEntry(char* name)
{
    FILE* fp_original = NULL;
    FILE* fp_copy = NULL;

    char c;
    int lineToDelete = 0;
    int line_num = 0;
    char buffer[MAX_NAME_LENGTH];
    int tempLine = 0;

   lineToDelete = entryExists(name);

    //create a copy-file and open it
    fp_copy = fopen("db_copy.txt", "w");
    fp_original = fopen(DBFILE, "r");

    c = getc(fp_original);

    while (c != EOF)
    {
        if (tempLine != lineToDelete)
        {
            //copy all lines in file copy.c
            putc(c, fp_copy);
        }

        if (c == '\n')
        {
            tempLine++;
        }

        c = getc(fp_original);
    }


    //close both the files.
    fclose(fp_original);
    fclose(fp_copy);

    //remove original file
    remove(DBFILE);

    //rename the file copy.c to original name
    rename("db_copy.txt", DBFILE);
}

int db_write(struct resource* entryList, int numOfEntries)
{
    /**Database will be created in directory: "build/src", if not already existing. */
    FILE* fp = NULL;

    for(int i = 0; i < numOfEntries; i++)
    {
        if(entryExists(entryList[i].name) != -1)
        {
            updateEntry(entryList[i].name);
            fp = fopen(DBFILE, "a");
            fprintf(fp, "%s %s\n", entryList[i].name, entryList[i].value);
            fclose(fp);
        }
        else
        {
            fp = fopen(DBFILE, "a");
            fprintf(fp, "%s %s\n", entryList[i].name, entryList[i].value);
            fclose(fp);
        }
    }

    return 0;
}

struct resource readEntry(int line)
{
    FILE* fp = NULL;
    int lineCount = 0;
    struct resource entry =
    {
        "",
        ""
    };

    char name[MAX_NAME_LENGTH];
    char value[MAX_VALUE_LENGTH];

    fp = fopen(DBFILE, "r");

    while (lineCount != line)
    {

        if(fscanf(fp, " %s %s", name, value) == EOF)
        {
            return entry;
        }

        lineCount++;
    }

    fscanf(fp, " %s %s", name, value);

    strcpy(entry.name, name);
    strcpy(entry.value, value);

    return entry;
}


int db_read(char** nameList, int nameCount, struct resource* entryList)
{

    int line = -1;

    for(int i = 0; i < nameCount; i++)
    {
        line = entryExists(nameList[i]);

        if( line != -1)
        {
            entryList[i] = readEntry(line);
        }
    }

    return 1;
}
