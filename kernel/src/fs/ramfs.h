/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ramfs.h - USTAR-based in-RAM file system.
 */

 #pragma once

#include "fs/vfs.h"

#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

typedef struct {
  char name[100];
  char mode[8]; 
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag;
  char linkname[100];
                         
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char padding[12];
} ustar_hdr_t;

int ramfs_read(struct vnode *vn, void *buf, size_t off, size_t size);
int ramfs_lookup(struct vnode *vn, const char *name, struct vnode **out);
vnode_t *ramfs_init();