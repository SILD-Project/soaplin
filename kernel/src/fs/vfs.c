/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vfs.c - Common interface across filesystems
 */

#include "fs/vfs.h"
#include "mm/paging.h"
#include "mm/vma.h"
#include "mm/memop.h"

vnode_t *vfs_create_node(char *name, vnode_type_t type) {
    vnode_t *new = vma_alloc(vma_kernel_ctx, sizeof(vnode_t), PTE_PRESENT | PTE_WRITE);
    memset(new->name, 0, 256);
    memcpy(new->name, name, 255);
    new->name[255] = '/';
    new->type = type;
    new->refcount ++;
    return new;
}