#pragma once

#include <stdint.h>

struct vfs {
  struct vfs *vfs_next;           /* next vfs in list */
  struct vfsops *vfs_op;          /* operations on vfs */
  struct vnode *vfs_vnodecovered; /* vnode we cover */
  int vfs_flag;                   /* flags */
  int vfs_bsize;                  /* native block size */
  uint64_t vfs_data;               /* private data */
};

struct vfsops {
  int (*vfs_mount)();
  int (*vfs_unmount)();
  int (*vfs_root)();
  int (*vfs_statfs)();
  int (*vfs_sync)();
  int (*vfs_fid)();
  int (*vfs_vget)();
};

enum vtype { VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VBAD };
struct vnode {
  uint16_t v_flag;               /* vnode flags */
  uint16_t v_count;              /* reference count */
  uint16_t v_shlockc;            /* # of shared locks */
  uint16_t v_exlockc;            /* # of exclusive locks */
  struct vfs *v_vfsmountedhere; /* covering vfs */
  struct vnodeops *v_op;        /* vnode operations */
  union {
    struct socket *v_Socket; /* unix ipc */
    struct stdata *v_Stream; /* stream */
  };
  struct vfs *v_vfsp; /* vfs we are in */
  enum vtype v_type;  /* vnode type */
  uint64_t v_data;     /* private data */
};

struct vnodeops {
  int (*vn_open)();
  int (*vn_close)();
  int (*vn_rdwr)();
  int (*vn_ioctl)();
  int (*vn_select)();
  int (*vn_getattr)();
  int (*vn_setattr)();
  int (*vn_access)();
  int (*vn_lookup)();
  int (*vn_create)();
  int (*vn_remove)();
  int (*vn_link)();
  int (*vn_rename)();
  int (*vn_mkdir)();
  int (*vn_rmdir)();
  int (*vn_readdir)();
  int (*vn_symlink)();
  int (*vn_readlink)();
  int (*vn_fsync)();
  int (*vn_inactive)();
  int (*vn_bmap)();
  int (*vn_strategy)();
  int (*vn_bread)();
  int (*vn_brelse)();
};