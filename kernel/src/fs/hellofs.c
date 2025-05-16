#include "fs/hellofs.h"
#include "fs/vfs.h"
#include <mm/liballoc/liballoc.h>
#include <mm/memop.h>
#include <lib/string.h>

static int hellofs_read(vnode_t *vn, void *buf, size_t off, size_t size) {
    if (!vn || !buf || size <= 0) {
        return -1;
    }

    const char *hello = "hello";
    const size_t hello_len = 5;  // strlen("hello")
    char *cbuf = (char *)buf;
    size_t bytes_written = 0;

    while (bytes_written < size) {
        size_t pos = off % hello_len;  // Position within "hello"
        size_t remaining = hello_len - pos;  // Remaining bytes in current "hello"
        size_t to_copy = (size - bytes_written < remaining) ? 
                        (size - bytes_written) : remaining;

        memcpy(cbuf + bytes_written, hello + pos, to_copy);
        bytes_written += to_copy;
        off += to_copy;
    }

    return bytes_written;
}

vnode_ops_t hellofs_hello_ops = {
    .read = hellofs_read,
    .lookup = NULL,
};

static int hellofs_lookup(vnode_t *vn, const char *name, vnode_t **out) {
    if (!vn || !name || !out) {
        return -1;
    }

    if (strcmp(name, "hello") == 0) {
        *out = vfs_create_node("hello", VN_FILE);
        if (*out) {
            (*out)->ops = &hellofs_hello_ops;
            return 0;
        }

        return -1;
    } else {
        *out = NULL; // Not found
        return -1;
    }
}

vnode_ops_t hellofs_root_ops = {
    .read = NULL,
    .lookup = hellofs_lookup,
};

static int hellofs_mount(fs_t *fs, vnode_t *mountpoint) {
    if (!fs || !mountpoint) {
        return -1;
    }

    // mountpoint list isn't implemented.

    /*mountpoint_t *mp = (mountpoint_t *)malloc(sizeof(mountpoint_t));
    if (!mp) {
        return -1;
    }

    memset(mp, 0, sizeof(mountpoint_t));
    strncpy(mp->name, fs->name, sizeof(mp->name) - 1);
    mp->fs = fs;
    mp->mountpoint = mountpoint;*/

    // Mounting is essentially just putting the file system's root directory operations
    // on the mountpoint vnode.
    mountpoint->ops = &hellofs_root_ops;

    return 0;
}

fs_t *hellofs_init() {
    fs_t *fs = (fs_t *)malloc(sizeof(fs_t));
    if (!fs) {
        return NULL;
    }
    
    memset(fs, 0, sizeof(fs_t));
    strncpy(fs->name, "hellofs", sizeof(fs->name) - 1);
    
    fs->mount = hellofs_mount;
    fs->root = vfs_create_node("/", VN_DIR);
    if (!fs->root) {
        free(fs);
        return NULL;
    }
    
    return fs;
}