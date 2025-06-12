/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ramfs.h - CPIO-based in-RAM file system.
 */

#pragma once

#include "fs/vfs.h"

#define CPIO_MAGIC "070701"

typedef struct {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
} cpio_newc_hdr_t;

int ramfs_read(struct vnode *vn, void *buf, size_t off, size_t size);
int ramfs_lookup(struct vnode *vn, const char *name, struct vnode **out);
vnode_t *ramfs_init();