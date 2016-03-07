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
    char* namePtr[MAX_NUM_OF_ENTRIES];

    printf("Enter commands: \n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1)
    {
        fgets(msg, MAX_MSG_LENGTH, stdin);

        nextToken = strtok(msg, " \n");
        numOfTokens = 0;

        while( (nextToken != NULL) && (numOfTokens < MAX_NUM_OF_TOKENS) )
        {
            strcpy(tokenList[numOfTokens],nextToken);
            nextToken = strtok(NULL, " \n");
            numOfTokens++;
        }

        commandType = getCommandType(numOfTokens, tokenList);
        resourceCount = 0;

        switch(commandType)
        {
            case WRITE_COMMAND:
                printf("WRITE command!\n");

                for(int i = 1; i < numOfTokens; i += 2)
                {
                    strcpy(entryList[resourceCount].name,  tokenList[i]);
                    strcpy(entryList[resourceCount].value, tokenList[i+1]);
                    resourceCount++;
                }

                store_write(resourceCount, entryList);

                break;

            case READ_COMMAND:
                printf("READ command!\n");

                for (int i = 1; i < numOfTokens; i++)
                {
                    namePtr[resourceCount] = tokenList[i];
                    resourceCount++;
                }

                store_read(resourceCount, namePtr, entryList);

                for(int i = 0; i < resourceCount; i++)
                {
                    printf(" %s:%s\n", entryList[i].name, entryList[i].value);
                }

                break;

            case UNKNOWN_COMMAND:
            default:
                printf("Unknown command!");
                break;
        }

        printf("\n");
        printf("\n");
    }
#pragma clang diagnostic pop
}

