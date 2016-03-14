#include <unistd.h>
#include "client.h"
#include <ctype.h>

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
    else if(strcmp(tokenList[0],"add") == 0)
    {
        return ADD_COMMAND;
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

    srand((unsigned int) time(NULL));
    printf("Enter commands: \n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    //strcpy(msg, "write A 1");
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
        int lockResult = 0;

        switch(commandType)
        {
            case WRITE_COMMAND:
            {
                for(int i = 1; i < numOfTokens; i += 2)
                {
                    lockResult = lock(tokenList[i]);
                    while (lockResult) {
                        usleep((__useconds_t) (rand() % 25 + 5));
                        lockResult = lock(tokenList[i]);
                    }

                    strcpy(entryList[resourceCount].name,  tokenList[i]);
                    strcpy(entryList[resourceCount].value, tokenList[i+1]);
                    resourceCount++;
                }

                store_write(resourceCount, entryList, 0);

                for (int i = resourceCount - 1; i >= 0; i--)
                    unlock(entryList[i].name);

                break;
            }
            case READ_COMMAND:
                for (int i = 1; i < numOfTokens; i++)
                {
                    lockResult = lock(tokenList[i]);
                    while (lockResult) {
                        usleep((__useconds_t) (rand() % 25 + 5));
                        lockResult = lock(tokenList[i]);
                    }

                    namePtr[resourceCount] = tokenList[i];
                    resourceCount++;
                }

                store_read(resourceCount, namePtr, entryList);

                for (int i = 0; i < resourceCount; i++) {
                    printf("\t%s:%s\n", entryList[i].name, entryList[i].value);
                }

                for (int i = resourceCount - 1; i >= 0; i--)
                    unlock(namePtr[i]);

                break;
            case ADD_COMMAND:
            {
                char* intNames[MAX_NUM_OF_ENTRIES];
                int intVals[MAX_NUM_OF_ENTRIES];
                char* valNames[MAX_NUM_OF_ENTRIES];
                char* valPtr[MAX_NUM_OF_ENTRIES];
                int foundDigit = 0;

                int currentInt = 0;
                int currentVal = 0;

                for (int i = 1; i < numOfTokens; i += 2)
                {
                    lockResult = lock(tokenList[i]);
                    while (lockResult) {
                        usleep((__useconds_t) (rand() % 25 + 5));
                        lockResult = lock(tokenList[i]);
                    }

                    if (isdigit(tokenList[i+1][0]))
                    {
                        foundDigit = 1;
                        intVals[resourceCount] = atoi(tokenList[i+1]);
                        intNames[resourceCount] = tokenList[i];
                        currentInt++;
                    }
                    else
                    {
                        valPtr[resourceCount] = tokenList[i+1];
                        lockResult = lock(tokenList[i+1]);
                        while (lockResult) {
                            usleep((__useconds_t) (rand() % 25 + 5));
                            lockResult = lock(tokenList[i+1]);
                        }
                        valNames[resourceCount] = tokenList[i];
                        currentVal++;
                    }
                }

                store_add_const(currentInt, intNames, intVals, false);
                store_add_var(currentVal, valNames, valPtr, false);
                for (int i = currentVal - 1; i >= 0; i--) {
                    unlock(valPtr[i]);
                    unlock(valNames[i]);
                }

                for (int i = currentInt - 1; i >= 0; i--)
                    unlock(intNames[i]);

                break;
            }
            case UNKNOWN_COMMAND:
            default:
                printf("Unknown command. Use \"read\" or \"write\"\n");
                break;
        }


        printf("DONE\n");
    }
#pragma clang diagnostic pop
}

