#include <unistd.h>
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
        int lockFailed = 0;

        switch(commandType)
        {
            case WRITE_COMMAND:
                for(int i = 1; i < numOfTokens; i += 2)
                {
                    if (lock(tokenList[i])) {
                        printf("Couldn't lock %s\n", tokenList[i]);
                        lockFailed = 1;
                        break;
                    }

                    printf("// %s LOCKED\n", tokenList[i]);
                    strcpy(entryList[resourceCount].name,  tokenList[i]);
                    strcpy(entryList[resourceCount].value, tokenList[i+1]);
                    resourceCount++;
                }

                if (!lockFailed)
                    store_write(resourceCount, entryList);

                for (int i = resourceCount - 1; i >= 0; i--)
                {
                    unlock(entryList[i].name);
                    printf("// %s UNLOCKED\n", entryList[i].name);
                }

                break;

            case READ_COMMAND:
                for (int i = 1; i < numOfTokens; i++)
                {
                    if (lock(tokenList[i])) {
                        printf("Couldn't lock %s\n", tokenList[i]);
                        lockFailed = 1;
                        break;
                    }

                    printf("// %s LOCKED\n", tokenList[i]);
                    namePtr[resourceCount] = tokenList[i];
                    resourceCount++;
                }

                if (!lockFailed) {
                    store_read(resourceCount, namePtr, entryList);

                    for (int i = 0; i < resourceCount; i++) {
                        printf("\t%s:%s\n", entryList[i].name, entryList[i].value);
                    }
                }

                for (int i = resourceCount - 1; i >= 0; i--)
                {
                    unlock(namePtr[i]);
                    printf("// %s UNLOCKED\n", namePtr[i]);
                }

                break;

            case UNKNOWN_COMMAND:
            default:
                printf("Unknown command. Use \"read\" or \"write\"\n");
                break;
        }

        printf("DONE\n");
    }
#pragma clang diagnostic pop
}

