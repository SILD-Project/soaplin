#pragma once

// btw fuck sun

#include <stdint.h>

#define VNODE_TYPE_FILE 0
#define VNODE_TYPE_DIR  1
#define VNODE_TYPE_DEV  2

typedef struct __vnode {
    char name[128];
    int  type;

    struct __vnode *children;
    struct __vnode *next;
    struct __vnode *parent;

    void(*write)(void *buffer, uint64_t off, uint64_t size);
    void(*read)(void *buffer, uint64_t off, uint64_t size);
} vnode;

typedef struct __vfs_mount {
    struct vfs_node *mount_point;       // The directory in the main VFS where this is mounted
    struct vfs_node *mounted_root;      // The root node of the mounted filesystem
    struct vfs_mount *next;             // Pointer to next mount point
} vfs_mount;

