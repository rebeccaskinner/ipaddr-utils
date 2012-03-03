#ifndef __IP_TREE_H__
#define __IP_TREE_H__
#include <inttypes.h>

#define IP_TREE_BLACK 0
#define IP_TREE_RED   1

/* use a specialized red-black tree for storing ip addresses so that we can get
 * back out a sorted list of them.
 */

struct ip_node;
struct ip_tree;

typedef struct ip_node ip_node_t;
typedef struct ip_tree ip_tree_t;

struct ip_node
{
    ip_node_t *left, *right;
    uint32_t  ipaddr;
    uintptr_t parent;
} __attribute__((aligned));

struct ip_tree
{
    ip_node_t* first;
    ip_node_t* last;
    ip_node_t* root;
} __attribute__((aligned));

#endif
