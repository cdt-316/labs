#include "database.h"

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
			printf("A match found on line: %d\n", line_num);
            lineToDelete = line_num;
            printf("line to delete: %d \n", lineToDelete);
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
        printf("Line: %d, Char: %d \n", tempLine, c );

        if (tempLine != lineToDelete)
        {
            printf("Char copied: %d \n", c);
            //copy all lines in file copy.c
            putc(c, fp_copy);
        }

        if (c == '\n')
        {
            tempLine++;
        }

        c = getc(fp_original);
    }

    printf("Line: %d \n", tempLine );

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

        printf("%d \n", i);

        if(entryExists(entryList[i].name) != -1)
        {
            printf("Name %s already exists! \n", entryList[i].name);
            updateEntry(entryList[i].name);
            fp = fopen(DBFILE, "a");
            fprintf(fp, "%s %s\n", entryList[i].name, entryList[i].value);
        }
        else
        {
            fp = fopen(DBFILE, "a");
            printf("Name %s did not exist! It will now be added. \n", entryList[i].name);
            fprintf(fp, "%s %s\n", entryList[i].name, entryList[i].value);
        }
    }

    fclose(fp);

}

