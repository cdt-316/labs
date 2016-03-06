//
// Created by mitch on 2/24/16.
//

#ifndef DISTRIBUTED_DB_CONFIG_H
#define DISTRIBUTED_DB_CONFIG_H
#define IP_ADDRESS_SIZE 16
#define MAX_NODES 16

struct node {
    int id;
    int port;
    char address[IP_ADDRESS_SIZE];
};

/**
 * Initializes config, loading the nodes into memory.
 * Returns -1 if something goes wrong
 */
int config_init(char* filename);

/**
 * Returns the total number of nodes in configuration, or -1 if config hasn't been set up yet
 */
int node_count();

/**
 * Returns the node that has the requested id.
 * Returns NULL if an error occurs
 */
struct node* node_for_id(int id);

/**
 * Returns the id of the node at the address and port specified.
 * If configuration hasn't been init, or the node couldn't be found returns -1
 */
int get_node_id(char *address, int port);

/**
 * Returns the first node with the same ip address as the curent machine
 * If the node couldn't be found, NULL is returned
 */
struct node* this_node();

#endif //DISTRIBUTED_DB_CONFIG_H
