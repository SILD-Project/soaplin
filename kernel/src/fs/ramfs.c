/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ramfs.c - USTAR-based in-RAM file system.
 */

#include "deps/limine.h"
#include "fs/vfs.h"
#include "lib/log.h"
#include "mm/paging.h"
#include "mm/vma.h"
#include <fs/ramfs.h>
#include <boot/limine.h>
#include <mm/memop.h>
#include <stdint.h>
#include <lib/string.h>

#define TARHDR_LIST_INCR 16
static ustar_hdr_t **_tarhdr_list = NULL;
static int _tarhdr_list_count = 0;
static int __ramfs_ustarhdr_last_inode = -1;

static void _ramfs_extend_tarhdr_list() {
    if (!_tarhdr_list && _tarhdr_list_count == 0) {
        _tarhdr_list_count += TARHDR_LIST_INCR;
        _tarhdr_list = vma_alloc(vma_kernel_ctx, sizeof(void*) * _tarhdr_list_count, PTE_PRESENT | PTE_WRITE);
    } else {
        _tarhdr_list_count += TARHDR_LIST_INCR;
        _tarhdr_list = vma_realloc(vma_kernel_ctx, _tarhdr_list, sizeof(void*) * _tarhdr_list_count, PTE_PRESENT | PTE_WRITE);
    }
}
static int __ramfs_octal_to_bin(unsigned char *str, int size) {
    int n = 0;
    for (int i = 0; i < size; i++) {
        n = (n << 3) + (str[i] - '0');
    }
    return n;
}
static void __ramfs_normalize_name(char *name) {
    if (name[0] == '.' && name[1] == '/')
        memmove(name, name + 1, strlen(name + 1) + 1);

    size_t len = strlen(name);
    if (len > 1 && name[len - 1] == '/')
        name[len - 1] = '\0';
}
static ustar_hdr_t *__ramfs_lookup_ustarhdr(const char *name, int size) {
    for (int i = 0; i < _tarhdr_list_count; i++) {
        ustar_hdr_t *hdr = _tarhdr_list[i];
        if (!hdr) continue;

        if (memcmp(hdr->name, name, size) == 0) {
            __ramfs_ustarhdr_last_inode = i;
            return hdr;
        }
    }
    __ramfs_ustarhdr_last_inode = 0;
    return NULL;
}

vnode_ops_t ramfs_dir_ops = {
    .lookup = ramfs_lookup,
    .read = NULL
};

vnode_ops_t ramfs_file_ops = {
    .lookup = NULL,
    .read = ramfs_read
};

int ramfs_read(struct vnode *vn, void *buf, size_t off, size_t size) {
    uint8_t *hdr_addr = (uint8_t *)__ramfs_lookup_ustarhdr(vn->name, strlen(vn->name));
    ustar_hdr_t *hdr = (ustar_hdr_t *)hdr_addr;

    uint32_t filesize = __ramfs_octal_to_bin((unsigned char*)hdr->size, 12);

    if (off >= filesize)
        return 0;

    if (off + size > filesize)
        size = filesize - off;

    uint8_t *filecontent_addr = hdr_addr + 512;

    memcpy(buf, filecontent_addr + off, size);

    return size;
}

int ramfs_lookup(struct vnode *vn, const char *name, struct vnode **out) {
    if (!out) return -1;
    *out = NULL;

    char namebuf[4096];
    int vnlen = strlen(vn->name);
    int namelen = strlen(name);
    bool has_slash = (vnlen > 0 && vn->name[vnlen - 1] == '/');
    int total_len = has_slash ? vnlen + namelen : vnlen + 1 + namelen;

    if (vnlen + 1 >= (int)sizeof(namebuf) || total_len + 1 >= (int)sizeof(namebuf))
        return error("ramfs: path too long\n"), -1;

    memcpy(namebuf, vn->name, vnlen);
    if (!has_slash) namebuf[vnlen] = '/';
    memcpy(namebuf + (has_slash ? vnlen : vnlen + 1), name, namelen);
    namebuf[total_len] = '\0';

    __ramfs_normalize_name(namebuf);

    size_t cmp_len = total_len + 1;
    if (cmp_len > 100) {
        warn("ramfs: lookup: truncated to 100 chars\n");
        cmp_len = 100;
    }

    trace("ramfs: namebuf = %s\n", namebuf);

    ustar_hdr_t *hdr = __ramfs_lookup_ustarhdr(namebuf, cmp_len);
    if (!hdr) {
        *out = NULL;
        return -1;
    }

    int vntype;
    switch (hdr->typeflag) {
        case REGTYPE: vntype = VN_FILE; break;
        case DIRTYPE: vntype = VN_DIR; break;
        default:
            error("ramfs: unsupported type: %d\n", hdr->typeflag);
            return -1;
    }

    vnode_t *new_node = vfs_create_node(namebuf, vntype);
    if (!new_node) return -1;
    new_node->inode = __ramfs_ustarhdr_last_inode;
    new_node->ops = (vntype == VN_DIR) ? &ramfs_dir_ops : &ramfs_file_ops;

    *out = new_node;
    return 0;
}

vnode_t *ramfs_init() {
    struct limine_file *rfs = limine_get_module(0);
    if (!rfs) return error("No ramfs provided!\n"), NULL;

    ustar_hdr_t *mhdr = (ustar_hdr_t *)rfs->address;
    if (memcmp(&mhdr->magic, "ustar", 5) != 0)
        return error("Module 0 isn't a tape archive! (magic: %s)\n", mhdr->magic), NULL;

    _ramfs_extend_tarhdr_list();

    uint64_t addr = (uint64_t)rfs->address;
    for (int i = 0;; i++) {
        ustar_hdr_t *hdr = (ustar_hdr_t *)addr;
        if (hdr->name[0] == '\0') break;
        __ramfs_normalize_name(hdr->name);

        if (i >= _tarhdr_list_count)
            _ramfs_extend_tarhdr_list();

        _tarhdr_list[i] = hdr;
        trace("ustar: Found '%s'\n", hdr->name);

        uint32_t size = __ramfs_octal_to_bin((unsigned char *)hdr->size, 12);
        addr += ((size / 512) + 1) * 512;
        if (size % 512) addr += 512;
    }

    vnode_t *root = vfs_create_node("/", VN_DIR);
    root->inode = -1;
    root->ops = &ramfs_dir_ops;
    root->parent = root;
    return root;
}
