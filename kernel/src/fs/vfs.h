/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vfs.h - Common interface across filesystems
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

struct vnode;

#define VN_FILE 1
#define VN_DIR 2

typedef uint32_t vnode_type_t;

typedef struct vnode_ops {
  int (*read)(struct vnode *vn, void *buf, size_t off, size_t size);
  int (*lookup)(struct vnode *vn, const char *name, struct vnode **out);
} vnode_ops_t;

typedef struct vnode {
  char name[256];
  int inode;
  vnode_type_t type;
  uint32_t refcount;
  struct vnode* parent; // If this vnode exists, then it's parent too.
  
  struct vnode_ops *ops;
  void *internal;
} vnode_t;

typedef struct mountpoint {
  char name[32];
  struct fs *fs;
  vnode_t *mountpoint;
} mountpoint_t;

typedef struct fs {
  char name[32];
  struct vnode *root;
  int (*mount)(struct fs *fs, struct vnode *mountpoint);
} fs_t;

void vfs_init(void);
int vfs_open(vnode_t *curdir, const char *path, vnode_t **out);
int vfs_read(vnode_t *vn, void *buf, size_t off, size_t size);
vnode_t *vfs_create_node(char *name, vnode_type_t type);