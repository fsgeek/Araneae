//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#include "hobofs.h"
#include "hobo.h"

// registration structure for operations we support

static struct fuse_lowlevel_ops hobo_ops;

const struct fuse_lowlevel_ops *hobo_getops(void)
{
    hobo_ops.init = hobo_init;
    hobo_ops.destroy = hobo_destroy;
    hobo_ops.lookup = hobo_lookup;
    hobo_ops.getattr = hobo_getattr;
    hobo_ops.opendir = hobo_opendir;
    hobo_ops.readdir = hobo_readdir;
    hobo_ops.releasedir = hobo_releasedir;
    hobo_ops.open = hobo_open;
    hobo_ops.access = hobo_access;

    /*
    extern hobo_init_t hobo_init;
    extern hobo_destroy_t hobo_destroy;
    extern hobo_lookup_t hobo_lookup;
    extern hobo_forget_t hobo_forget;
    extern hobo_getattr_t hobo_getattr;
    extern hobo_setattr_t hobo_setattr;
    extern hobo_readlink_t hobo_readlink;
    extern hobo_mknod_t hobo_mknod;
    extern hobo_mkdir_t hobo_mkdir;
    extern hobo_unlink_t hobo_unlink;
    extern hobo_rmdir_t hobo_rmdir;
    extern hobo_symlink_t hobo_symlink;
    extern hobo_rename_t hobo_rename;
    extern hobo_link_t hobo_link;
    extern hobo_open_t hobo_open;
    extern hobo_read_t hobo_read;
    extern hobo_write_t hobo_write;
    extern hobo_flush_t hobo_flush;
    extern hobo_release_t hobo_release;
    extern hobo_fsync_t hobo_fsync;
    extern hobo_opendir_t hobo_opendir;
    extern hobo_readdir_t hobo_readdir;
    extern hobo_releasedir_t hobo_releasedir;
    extern hobo_fsyncdir_t hobo_fsyncdir;
    extern hobo_statfs_t hobo_statfs;
    extern hobo_setxattr_t hobo_setxattr;
    extern hobo_getxattr_t hobo_getxattr;
    extern hobo_listxattr_t hobo_listxattr;
    extern hobo_removexattr_t hobo_removexattr;
    extern hobo_access_t hobo_access;
    extern hobo_create_t hobo_create;
    extern hobo_getlk_t hobo_getlk;
    extern hobo_setlk_t hobo_setlk;
    extern hobo_bmap_t hobo_bmap;
    extern hobo_ioctl_t hobo_ioctl;
    extern hobo_poll_t hobo_poll;
    extern hobo_write_buf_t hobo_write_buf;
    extern hobo_retrieve_reply_t hobo_retrieve_reply;
    extern hobo_forget_multi_t hobo_forget_multi;
    extern hobo_flock_t hobo_flock;
    extern hobo_fallocate_t hobo_fallocate;
    extern hobo_readdirplus_t hobo_readdirplus;
    extern hobo_copy_file_range_t hobo_copy_file_range;

    */

    return &hobo_ops;
}
