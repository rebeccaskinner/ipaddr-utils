/* Copyright Rebecca Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#include <stdlib.h>
#include <stdio.h>
#include "iptree.h"

/* Based off Franck Bui-Huu's libtree code: https://github.com/fbuihuu/libtree
 * Licensed under the terms of the LGPL */

#define POSITIVE(x) ((x) > 0)

static inline int ipnode_get_color(ip_node_t* node)
{
    return (node->parent & 1);
}

static inline ip_node_t* ipnode_set_color(ip_node_t* node, int color)
{
    node->parent = (node->parent & (~1UL)) | color;
    return node;
}

static inline ip_node_t* ipnode_get_parent(ip_node_t* node)
{
    return (ip_node_t*)(node->parent & (~1UL));
}

static inline void ipnode_set_parent(ip_node_t* parent, ip_node_t* node)
{
    node->parent = (uintptr_t)parent | (node->parent & 1);
}

static inline int ipnode_is_root(ip_node_t* node)
{
    return (NULL == ipnode_get_parent(node));
}

static inline int ipnode_is_black(ip_node_t* node)
{
    return (IP_TREE_BLACK == ipnode_get_color(node));
}

static inline int ipnode_is_red(ip_node_t* node)
{
    return (IP_TREE_RED == ipnode_get_color(node));
}

static inline ip_node_t* ipnode_first(ip_node_t* node)
{
    for(; node->left; node = node->left);
    return node;
}

static inline ip_node_t* ipnode_last(ip_node_t* node)
{
    for(; node->right; node = node->right);
    return node;
}

ip_node_t* iptree_first(ip_tree_t* tree)
{
    return tree->first;
}

ip_node_t* iptree_last(ip_tree_t* tree)
{
    return tree->last;
}

ip_node_t* iptree_get_first(ip_node_t* node)
{
    while (node->left) {
        node = node->left;
    }
    return node;
}

ip_node_t* iptree_get_last(ip_node_t* node)
{
    while (node->right) {
        node = node->right;
    }
    return node;
}

ip_node_t* iptree_next(ip_node_t* node)
{
    ip_node_t* parent;
    if(node->right) return ipnode_first(node->right);
    while((parent = ipnode_get_parent(node)) && (parent->right == node))
        node = parent;
    return parent;
}

ip_node_t* iptree_prev(ip_node_t* node)
{
    ip_node_t* parent;
    if (node->left) {
        return iptree_get_last(node->left);
    }
    while ((parent = ipnode_get_parent(node)) && parent->left == node) {
        node = parent;
    }
    return parent;
}

static inline ip_node_t* ipnode_lookup(ip_node_t* key,
                                       ip_tree_t* tree,
                                       ip_node_t** parent,
                                       int* left)
{
    ip_node_t* node = tree->root;
    *parent = NULL;
    *left = 0;

    while(node)
    {
        intptr_t diff = ((intptr_t)(node->ipaddr) - (intptr_t)(key->ipaddr));
        if(!diff) return node;
        *parent = node;
        node = (0 == (*left = POSITIVE(diff))) ? node->right : node->left;
    }

    return NULL;
}

static void iptree_rotate_left(ip_node_t* node, ip_tree_t* tree)
{
    ip_node_t* a = node;
    ip_node_t* b = node->right;
    ip_node_t* parent = ipnode_get_parent(node);

    if(ipnode_is_root(a))
    {
        tree->root = b;
    }
    else
    {
        if(parent->left == a)
            parent->left = b;
        else
            parent->right = b;
    }
    ipnode_set_parent(parent, b);
    ipnode_set_parent(b,a);
    if((a->right = b->left))
    {
        ipnode_set_parent(a,a->right);
    }
    b->left = a;
}

static void iptree_rotate_right(ip_node_t* node, ip_tree_t* tree)
{
    ip_node_t* a = node;
    ip_node_t* b = node->left;
    ip_node_t* parent = ipnode_get_parent(node);

    if(ipnode_is_root(a))
    {
        tree->root = b;
    }
    else
    {
        if(parent->left == a) {
            parent->left = b;
        }
        else {
            parent->right = b;
        }
    }
    ipnode_set_parent(parent, b);
    ipnode_set_parent(b, a);
    if((a->left = b->right))
    {
        ipnode_set_parent(a,a->left);
    }
    b->right = a;
}

ip_node_t* iptree_find(ip_node_t* key, ip_tree_t* tree)
{
    ip_node_t* parent;
    int is_left;
    return ipnode_lookup(key, tree, &parent, &is_left);
}

int iptree_addr_exists(uint32_t addr, ip_tree_t* tree)
{
    ip_node_t key = {NULL,NULL,0,addr,0};
    return (NULL != (iptree_find(&key,tree)));
}

static void ipnode_set_child(ip_node_t* child, ip_node_t* node, int left)
{
    if(left) node->left = child;
    else node->right = child;
}

ip_node_t* iptree_insert(ip_node_t* node, ip_tree_t* tree, int flags)
{
    ip_node_t* key;
    ip_node_t* parent;
    int is_left;

    if((key = ipnode_lookup(node, tree, &parent, &is_left))) {
        if(flags & IPTREE_INCREMENT_REFCOUNT) {
            (key->refcount)++;
        }
        return key;
    }

    node->left = NULL;
    node->right = NULL;
    ipnode_set_color(node, IP_TREE_RED);
    ipnode_set_parent(parent, node);

    if(parent)
    {
        if(is_left)
        {
            if(parent == tree->first)
                tree->first = node;
        }
        else
        {
            if(parent == tree->last)
                tree->last = node;
        }
        ipnode_set_child(node, parent, is_left);
    }
    else
    {
        tree->root  = node;
        tree->first = node;
        tree->last  = node;
    }

    while((parent = ipnode_get_parent(node)) && ipnode_is_red(parent))
    {
        ip_node_t* grandparent = ipnode_get_parent(parent);
        ip_node_t* tmp = (parent == grandparent->left)?parent->right:parent->left;
        if(tmp && ipnode_is_red(tmp))
        {
            ipnode_set_color(tmp, IP_TREE_BLACK);
            node = grandparent;
        }
        else
        {
            if(parent == grandparent->left)
            {
                if(node == parent->right)
                {
                    iptree_rotate_left(parent, tree);
                    node = parent;
                    parent = ipnode_get_parent(parent);
                }
                iptree_rotate_right(grandparent, tree);
            }
            else
            {
                if(node == parent->left)
                {
                    iptree_rotate_right(parent, tree);
                    node = parent;
                    parent = ipnode_get_parent(parent);
                }
                iptree_rotate_left(grandparent, tree);
            }
        }

        ipnode_set_color(parent, IP_TREE_BLACK);
        ipnode_set_color(grandparent, IP_TREE_RED);
    }
    ipnode_set_color(tree->root, IP_TREE_BLACK);
    return NULL;
}

void iptree_remove(ip_node_t* node, ip_tree_t* tree)
{
    ip_node_t* parent = ipnode_get_parent(node);
    ip_node_t* left   = node->left;
    ip_node_t* right  = node->right;
    ip_node_t* next;
    int color;

    if (node == tree->first) {
        tree->first = iptree_next(node);
    }
    if (node == tree->last) {
        tree->last = iptree_prev(node);
    }

    if (!left) {
        next = right;
    }
    else if (!right) {
        next = left;
    }
    else {
        next = iptree_get_first(right);
    }

    if (parent) {
        ipnode_set_child(next, parent, parent->left == node);
    }
    else {
        tree->root = next;
    }

    if (left && right) {
        color = ipnode_get_color(next);
        ipnode_set_color(next, ipnode_get_color(node));

        next->left = left;
        ipnode_set_parent(next, left);

        if (next != right) {
            parent = ipnode_get_parent(next);
            ipnode_set_parent(ipnode_get_parent(node), next);

            node = next->right;
            parent->left = node;

            next->right = right;
            ipnode_set_parent(next, right);
        }
        else {
            ipnode_set_parent(parent, next);
            parent = next;
            node = next->right;
        }
    }
    else {
        color = ipnode_get_color(node);
        node = next;
    }
    if (node) {
        ipnode_set_parent(parent, node);
    }

    if (color == IP_TREE_RED) {
        return;
    }
    if (node && ipnode_is_red(node)) {
        ipnode_set_color(node, IP_TREE_BLACK);
        return;
    }

    do {
        if (node == tree->root)
        {
            break;
        }

        if (node == parent->left) {
            ip_node_t* sibling = parent->right;

            if (ipnode_is_red(sibling)) {
                ipnode_set_color(sibling, IP_TREE_BLACK);
                ipnode_set_color(parent, IP_TREE_RED);
                iptree_rotate_left(parent, tree);
                sibling = parent->right;
            }
            if ((!sibling->left  || ipnode_is_black(sibling->left)) &&
                    (!sibling->right || ipnode_is_black(sibling->right))) {
                ipnode_set_color(sibling, IP_TREE_RED);
                node = parent;
                parent = ipnode_get_parent(parent);
                continue;
            }
            if (!sibling->right || ipnode_is_black(sibling->right)) {
                ipnode_set_color(sibling->left, IP_TREE_BLACK);
                ipnode_set_color(sibling, IP_TREE_RED);
                iptree_rotate_right(sibling, tree);
                sibling = parent->right;
            }
            ipnode_set_color(sibling, ipnode_get_color(parent));
            ipnode_set_color(parent, IP_TREE_BLACK);
            ipnode_set_color(sibling->right, IP_TREE_BLACK);
            iptree_rotate_left(parent, tree);
            node = tree->root;
            break;
        }
        else {
            ip_node_t* sibling = parent->left;

            if (ipnode_is_red(sibling)) {
                ipnode_set_color(sibling, IP_TREE_BLACK);
                ipnode_set_color(parent, IP_TREE_RED);
                iptree_rotate_right(parent, tree);
                sibling = parent->left;
            }
            if ((!sibling->left  || ipnode_is_black(sibling->left)) &&
                    (!sibling->right || ipnode_is_black(sibling->right))) {
                ipnode_set_color(sibling, IP_TREE_RED);
                node = parent;
                parent = ipnode_get_parent(parent);
                continue;
            }
            if (!sibling->left || ipnode_is_black(sibling->left)) {
                ipnode_set_color(sibling->right, IP_TREE_BLACK);
                ipnode_set_color(sibling, IP_TREE_RED);
                iptree_rotate_left(sibling, tree);
                sibling = parent->left;
            }
            ipnode_set_color(sibling, ipnode_get_color(parent));
            ipnode_set_color(parent, IP_TREE_BLACK);
            ipnode_set_color(sibling->left, IP_TREE_BLACK);
            iptree_rotate_right(parent, tree);
            node = tree->root;
            break;
        }
    } while (ipnode_is_black(node));

    if (node) {
        ipnode_set_color(node, IP_TREE_BLACK);
    }
}

ip_tree_t* iptree_new()
{
    ip_tree_t* tree = malloc(sizeof(ip_tree_t));
    tree->root  = NULL;
    tree->first = NULL;
    tree->last  = NULL;
    return tree;
}

ip_node_t* ipnode_new(uint32_t addr)
{
    ip_node_t* node = malloc(sizeof(ip_node_t));
    node->left     = NULL;
    node->right    = NULL;
    node->parent   = 0;
    node->refcount = 1;
    node->ipaddr   = addr;
    return node;
}

ip_node_t* iptree_add_addr(uint32_t addr, ip_tree_t* tree, int flags)
{
    ip_node_t* node = ipnode_new(addr);
    ip_node_t* new_node = iptree_insert(node, tree, flags);
    if(!new_node){
        new_node = node;
    }
    else{
        free(node);
    }
    return new_node;
}

void iptree_rm_addr(uint32_t addr, ip_tree_t* tree)
{
    ip_node_t* parent  = NULL;
    ip_node_t  key     = {NULL,NULL,1,addr,0};
    int        is_left = 0;
    ip_node_t* node = ipnode_lookup(&key, tree, &parent, &is_left);
    if(!node) {
        goto cleanup;
    }
    (node->refcount)--;
    if(!node->refcount) {
        iptree_remove(node, tree);
        free(node);
    }
cleanup:
    return;
}

size_t iptree_get_sorted(ip_tree_t* tree, uint32_t* elems)
{
    ip_node_t* node = tree->first;
    size_t count = 0;
    for(; node; node = iptree_next(node))
    {
        elems[count++] = node->ipaddr;
    }
    return count;
}
