/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ramfs.c - CPIO-based in-RAM file system.
 */

#include "deps/limine.h"
#include "fs/vfs.h"
#include "lib/hashmap.h"
#include "lib/log.h"
#include "mm/paging.h"
#include "mm/vma.h"
#include <fs/ramfs.h>
#include <boot/limine.h>
#include <mm/memop.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <lib/string.h>

#define _ramfs_align4(x) (((x) + 3) & ~3)

static void _ramfs_combine_path(char *out, const char *path1, const char *path2) {
    if (!out)
        return;

    const int max_len = 256;
    int len1 = 0;
    int len2 = 0;

    if (path1) {
        while (len1 < max_len && path1[len1] != '\0') {
            len1++;
        }
    }

    if (path2) {
        while (len2 < max_len && path2[len2] != '\0') {
            len2++;
        }
    }

    int pos = 0;
    if (len1 > 0) {
        if (len1 > max_len)
            len1 = max_len;
        memcpy(out + pos, path1, len1);
        pos += len1;
    }

    if (pos > 0 && out[pos -1] != '/' && len2 > 0 && pos < max_len -1) {
        out[pos] = '/';
        pos++;
    }

    if (len2 > 0) {
        int to_copy = max_len - pos - 1;
        if (to_copy > len2)
            to_copy = len2;
        memcpy(out + pos, path2, to_copy);
        pos += to_copy;
    }

    if (pos >= max_len)
        pos = max_len - 1;
    out[pos] = '\0';
}

static hashmap_t hdr_map;

static vnode_ops_t ramfs_file_ops = {
    .lookup = ramfs_lookup,
    .read   = ramfs_read
};

int ramfs_read(struct vnode *vn, void *buf, size_t off, size_t size) {
    // TODO: offset is ignored
    if (!vn || !buf || size == 0)
        return -1;

    cpio_newc_hdr_t *hdr = hashmap_get(&hdr_map, &vn->name[1]);
    if (!hdr)
        return -2;

    int file_name_size = strtoul(hdr->c_namesize, 8);
    int file_size = strtoul(hdr->c_filesize, 8);
    int read_size = size;
    if (file_size <= read_size)
        read_size = file_size;
    
    const char *contents_addr = (const char *)((uint8_t *)hdr + _ramfs_align4(sizeof(cpio_newc_hdr_t) + file_name_size));
    memcpy(buf, contents_addr, read_size);
    return read_size;
}

int ramfs_lookup(struct vnode *vn, const char *name, struct vnode **out) {
    if (!out)
        return -1;

    char _temp_name_buf[256];
    _ramfs_combine_path(_temp_name_buf, vn->name, name);

    cpio_newc_hdr_t *hdr = hashmap_get(&hdr_map, &_temp_name_buf[1]); // CPIO uses filenames without / at the start.
    if (!hdr) {
        *out = NULL;
        return -1;
    }

    trace("ramfs: Found '%s' (inode %d)!\n", _temp_name_buf, strtoul(hdr->c_ino, 8));
    int vntype = -1;

    switch (strtoul(hdr->c_mode, 8) & 0170000) {
        case 0100000: vntype = VN_FILE; break;
        case 0040000: vntype = VN_DIR;  break;
        default:
            error("ramfs: '%s' has an unsupported type.\n", _temp_name_buf);
            *out = NULL;
            return -1;
    }

    vnode_t *vn_child = vfs_create_node(_temp_name_buf, vntype);
    vn_child->parent = vn;
    vn_child->ops = &ramfs_file_ops;
    vn_child->inode = strtoul(hdr->c_ino, 8);
    *out = vn_child;
    return 0;
}

vnode_t *ramfs_init() {
    struct limine_file *cpio = limine_get_module(0);
    if (!cpio) {
        error("ramfs: No CPIO archive provided!\n");
        return NULL;
    }

    hashmap_init(&hdr_map, 16);
    cpio_newc_hdr_t *hdr = (cpio_newc_hdr_t*)cpio->address;

    while (1) {
        if (memcmp(hdr->c_magic, CPIO_MAGIC, 6) != 0) {
            error("ramfs: Magic check failed. Did you provide a CPIO newc archive?\n");
            continue;
        }

        if (memcmp((uint8_t*)hdr + sizeof(cpio_newc_hdr_t), "TRAILER!!!", 10) == 0)
            break;

        uint32_t namesize = strtoul(hdr->c_namesize, 8);
        uint32_t filesize = strtoul(hdr->c_filesize, 8);

        hashmap_put(&hdr_map, (const char *)((uint8_t*)hdr + sizeof(cpio_newc_hdr_t)), hdr);
        trace("cpio: Found %s\n", (uint8_t*)hdr + sizeof(cpio_newc_hdr_t));

        hdr = (cpio_newc_hdr_t*)((uint8_t*)hdr + _ramfs_align4(sizeof(cpio_newc_hdr_t) + namesize) + _ramfs_align4(filesize));
    }

    vnode_t *root = vfs_create_node("/", VN_DIR);
    root->inode = 0;
    root->parent = root;
    root->ops = &ramfs_file_ops;
    return root;
}