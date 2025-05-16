#include "fs/vfs.h"
#include "lib/string.h"
#include "mm/liballoc/liballoc.h"
#include "mm/memop.h"
#include "sys/errhnd/panic.h"

vnode_t *root = NULL;

void vfs_init() {
  root = vfs_create_node("/", VN_DIR);
  if (!root) {
    panic("vfs - failed to create root node");
  }
  root->parent = root; // Root's parent is itself
}

// Not worth adding to the header, since it should only lookup
// for files/directories in dir, without going deeper.
int vfs_lookup(vnode_t *dir, const char *name, vnode_t **out) {
  if (!dir || !name)
    return -1;

  if (dir->ops && dir->ops->lookup) {
    return dir->ops->lookup(dir, name, out);
  }
  return -1;
}

int vfs_open(vnode_t *curdir, const char *path, vnode_t **out) {
  if (strcmp(path, ".") == 0) {
    *out = curdir;
    return 0;
  }
  if (strcmp(path, "..") == 0) {
    *out = curdir->parent;
    return 0;
  }

  char *path_copy = strdup(path);
  if (!path_copy) {
    *out = NULL;
    return -1;
  }

  vnode_t *cur_node = path[0] == '/' ? root : curdir;
  char *token = strtok(path_copy, "/");

  while (token) {
    vnode_t *next;
    vfs_lookup(cur_node, token, &next);
    if (!next) {
      free(path_copy);
      *out = NULL;
      return -1;
    }
    cur_node = next;
    token = strtok(NULL, "/");
  }

  free(path_copy);
  *out = cur_node;
  return 0;
}

int vfs_mount(char *path, fs_t *fs) {
  if (!fs || !path) {
    return -1;
  }

  vnode_t *mp;
  if (strcmp(path, "/") == 0) {
    mp = root;
  } else {
    vfs_open(root, path, &mp);
  }

  if (fs->mount == NULL)
    return -1; // why allocating a fs without the capability to mount to a node? lmao

  return fs->mount(fs, mp);
}

int vfs_unmount(char *path) {
  (void)path;
  return -1;
}

int vfs_read(vnode_t *vn, void *buf, size_t off, size_t size) {
  if (!vn || !buf || off < 0 || size <= 0) {
    return -1;
  }

  if (vn->ops && vn->ops->read) {
    return vn->ops->read(vn, buf, off, size);
  }
  return -1;
}

vnode_t *vfs_create_node(char *name, vnode_type_t type) {
  vnode_t *node = (vnode_t *)malloc(sizeof(vnode_t));
  if (!node) {
    return NULL;
  }
  memset(node, 0, sizeof(vnode_t));
  strncpy(node->name, name, sizeof(node->name) - 1);
  node->type = type;
  node->ops = NULL;
  // node->parent = NULL;
  // node->child = NULL;
  // node->next = NULL;
  return node;
}