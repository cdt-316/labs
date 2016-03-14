#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include "store.h"
#include <pthread.h>
#include <string.h>

#define MAX_MSG_LENGTH 50
#define MAX_NUM_OF_TOKENS 11
#define MAX_ARG_LENGTH 10
#define UNKNOWN_COMMAND -1
#define WRITE_COMMAND 0
#define READ_COMMAND 1
#define ADD_COMMAND 2

/**Checks if the tokenList can be interpreted as a valid command.
 * Returns (0) if Read command, (1) if Write command or (-1) if invalid command.*/
int parse(char tokenList[MAX_NUM_OF_TOKENS][MAX_ARG_LENGTH]);

/**Reads commands from user, and sends them to the mid-ware.*/
void run_client();

#endif // CLIENT_H_INCLUDED
