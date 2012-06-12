/* Copyright Tim Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#ifndef __IP_TREE_H__
#define __IP_TREE_H__
#include <inttypes.h>

#define IP_TREE_BLACK 0
#define IP_TREE_RED   1

/* Based off Franck Bui-Huu's libtree code: https://github.com/fbuihuu/libtree
 * Licensed under the terms of the LGPL */

/* use a specialized red-black tree for storing ip addresses so that we can get
 * back out a sorted list of them.
 */

/* If IPTREE_INCREMENT_REFCOUNT is set, then iptree_insert will
 * increase the refcount for any address inserted into the tree,
 * otherwise the refcount will be set to 1 if this is the first
 * time the addresses is inserted, and will otherwise not touch
 * the refcount */
#define IPTREE_INCREMENT_REFCOUNT (1<<0)
#define IPTREE_NO_INCREMENT_REFCOUNT (0)

struct ip_node;
struct ip_tree;

typedef struct ip_node ip_node_t;
typedef struct ip_tree ip_tree_t;

struct ip_node
{
    ip_node_t *left;
    ip_node_t *right;
    uint8_t   refcount;
    uint32_t  ipaddr;
    uintptr_t parent;
} __attribute__((aligned));

struct ip_tree
{
    ip_node_t* first;
    ip_node_t* last;
    ip_node_t* root;
} __attribute__((aligned));


ip_node_t* iptree_add_addr(uint32_t addr, ip_tree_t* tree, int flags);
size_t iptree_get_sorted(ip_tree_t* tree, uint32_t* elems);
ip_tree_t* iptree_new();
ip_node_t* iptree_find(ip_node_t* key, ip_tree_t* tree);
int iptree_addr_exists(uint32_t addr, ip_tree_t* tree);
void iptree_rm_addr(uint32_t addr, ip_tree_t* tree);

#endif
