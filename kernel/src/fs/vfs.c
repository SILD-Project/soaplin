#include "fs/vfs.h"
#include "mm/liballoc/liballoc.h"
#include "mm/memop.h"
#include "lib/string.h"

vnode_t *vfs_create_node(char *name, vnode_type_t type) {
    vnode_t *node = (vnode_t *)malloc(sizeof(vnode_t));
    if (!node) {
        return NULL;
    }
    memset(node, 0, sizeof(vnode_t));
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->type = type;
    node->ops = NULL;
    //node->parent = NULL;
    //node->child = NULL;
    //node->next = NULL;
    return node;
}