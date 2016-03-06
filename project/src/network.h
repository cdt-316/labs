//
// Created by mitch on 3/6/16.
//

#ifndef DISTRIBUTED_DB_NETWORK_H
#define DISTRIBUTED_DB_NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"

struct connection {
    struct node* node;
    struct sockaddr* socket;
};

/**
 * Connects to each possible node, and leaves a socket open for other nodes to connect to.
 * Maintains the new connections.
 */
void network_init();

#endif //DISTRIBUTED_DB_NETWORK_H
