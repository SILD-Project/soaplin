#pragma once

#include <stddef.h>
#include <stdint.h>

struct vnode;

#define VN_FILE 1
#define VN_DIR  2

typedef uint32_t vnode_type_t;

typedef struct vnode_ops {
    int (*read)(struct vnode* vn, void* buf, size_t size);
} vnode_ops_t;

typedef struct vnode {
    char name[256];
    vnode_type_t type;
    struct vnode* parent;
    struct vnode* child;
    struct vnode* next;

    struct vnode_ops* ops;
    void* internal;
} vnode_t;

typedef struct fs {
    char name[32];
    int (*mount)(struct vnode** root);
} fs_t;

void vfs_init(void);
int vfs_mount(char *path, fs_t* fs);
int vfs_unmount(char *path);
int vfs_open(const char* path, vnode_t** result);
int vfs_read(vnode_t* vn, void* buf, size_t size);
