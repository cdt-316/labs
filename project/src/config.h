//
// Created by mitch on 2/24/16.
//

#ifndef DISTRIBUTED_DB_CONFIG_H
#define DISTRIBUTED_DB_CONFIG_H
#define IP_ADDRESS_SIZE 16

struct node {
    int id;
    int port;
    char address[IP_ADDRESS_SIZE];
};

/**
 * Returns the total number of nodes in configuration.
 * If an error occurs, returns -1
 */
int node_count(char* filename);

/**
 * Returns the id of the ip at the address and port specified.
 * If an error occurs, or the node couldn't be found, -1 is returned.
 */
int id(char* filename, char* address, int port);

/**
 * Returns the data of the node with the address specified.
 * If an error occurs, or the node couldn't be found, NULL is returned
 */
struct node* this_node(char* filename);

#endif //DISTRIBUTED_DB_CONFIG_H
