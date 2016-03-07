#include "client.h"

int getCommandType(int numOfTokens, char tokenList[MAX_NUM_OF_TOKENS][MAX_ARG_LENGTH])
{
    if( strcmp(tokenList[0],"write") == 0)
    {
        if( (numOfTokens%2 != 0) && (numOfTokens > 2) )
            return WRITE_COMMAND;
        else
            return UNKNOWN_COMMAND;
    }
    else if( strcmp(tokenList[0],"read") == 0 )
    {
        if(numOfTokens > 1)
            return READ_COMMAND;
        else
            return UNKNOWN_COMMAND;
    }
    else
        return UNKNOWN_COMMAND;
}

void run_client()
{
    char msg[MAX_MSG_LENGTH];
    char tokenList[MAX_NUM_OF_TOKENS][MAX_ARG_LENGTH];
    char* nextToken;
    int numOfTokens = 0;
    int commandType = 0;
    int resourceCount = 0;
    struct resource entryList[MAX_NUM_OF_ENTRIES];
    char nameList[MAX_NUM_OF_ENTRIES][MAX_ARG_LENGTH];

    printf("Enter commands: \n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1)
    {
        fgets(msg, MAX_MSG_LENGTH, stdin);

        nextToken = strtok(msg, " ");
        numOfTokens = 0;

        while( (nextToken != NULL) && (numOfTokens < MAX_NUM_OF_TOKENS) )
        {
            strcpy(tokenList[numOfTokens],nextToken);
            nextToken = strtok(NULL, " ");
            numOfTokens++;
        }

        //remove later
        for(int i = 0; i < numOfTokens; i++)
        {
            printf("Arg[%d] is: %s", i, tokenList[i]);
            printf("\n");
        }

        //remove later
        printf("Number of tokens read: %d", numOfTokens);
        printf("\n");

        commandType = getCommandType(numOfTokens, tokenList);
        resourceCount = 0;

        switch(commandType)
        {
            case WRITE_COMMAND:
            printf("WRITE command!");

            for(int i = 1; i < numOfTokens; i += 2)
            {
                strcpy(entryList[resourceCount].name,  tokenList[i]);
                strcpy(entryList[resourceCount].value, tokenList[i+1]);
                resourceCount++;
            }

            store_write(resourceCount, entryList);

            break;

            case READ_COMMAND:
            printf("READ command!");

             for(int i = 1; i < numOfTokens; i++)
            {
                strcpy(nameList[resourceCount],  tokenList[i]);
                resourceCount++;
            }

            store_read(resourceCount, nameList, entryList);

            break;

            case UNKNOWN_COMMAND:
            printf("Unknown command!");

            break;

            default:
            printf("Unknown command!");
            break;
        }

        printf("\n");
        printf("\n");
    }
#pragma clang diagnostic pop
}

