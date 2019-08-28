//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#pragma once

#if !defined(__HOBO_H__)
#define __HOBO_H__ (1)

#include "hobofs.h"
#include "ino-lookup.h"

typedef void (*hobo_init_t)(void *userdata, struct fuse_conn_info *conn);
typedef void (*hobo_destroy_t)(void *userdata);
typedef void (*hobo_lookup_t)(fuse_req_t req, fuse_ino_t parent, const char *name);
typedef void (*hobo_forget_t)(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup);
typedef void (*hobo_getattr_t)(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_setattr_t) (fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi);
typedef void (*hobo_readlink_t) (fuse_req_t req, fuse_ino_t ino);
typedef void (*hobo_mknod_t) (fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev);
typedef void (*hobo_mkdir_t) (fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode);
typedef void (*hobo_unlink_t) (fuse_req_t req, fuse_ino_t parent, const char *name);
typedef void (*hobo_rmdir_t) (fuse_req_t req, fuse_ino_t parent, const char *name);
typedef void (*hobo_symlink_t) (fuse_req_t req, const char *link, fuse_ino_t parent, const char *name);
typedef void (*hobo_rename_t) (fuse_req_t req, fuse_ino_t parent, const char *name, fuse_ino_t newparent, const char *newname, unsigned int flags);
typedef	void (*hobo_link_t) (fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent, const char *newname);
typedef void (*hobo_open_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_read_t) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
typedef void (*hobo_write_t) (fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size, off_t off, struct fuse_file_info *fi);
typedef void (*hobo_flush_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_release_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_fsync_t) (fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi);
typedef void (*hobo_opendir_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_readdir_t) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
typedef	void (*hobo_releasedir_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
typedef void (*hobo_fsyncdir_t) (fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi);
typedef void (*hobo_statfs_t) (fuse_req_t req, fuse_ino_t ino);
typedef void (*hobo_setxattr_t) (fuse_req_t req, fuse_ino_t ino, const char *name, const char *value, size_t size, int flags);
typedef void (*hobo_getxattr_t) (fuse_req_t req, fuse_ino_t ino, const char *name, size_t size);
typedef void (*hobo_listxattr_t) (fuse_req_t req, fuse_ino_t ino, size_t size);
typedef	void (*hobo_removexattr_t) (fuse_req_t req, fuse_ino_t ino, const char *name);
typedef	void (*hobo_access_t) (fuse_req_t req, fuse_ino_t ino, int mask);
typedef void (*hobo_create_t) (fuse_req_t req, fuse_ino_t parent, const char *name,	mode_t mode, struct fuse_file_info *fi);
typedef void (*hobo_getlk_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock);
typedef	void (*hobo_setlk_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock, int sleep);
typedef void (*hobo_bmap_t) (fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx);
typedef void (*hobo_ioctl_t) (fuse_req_t req, fuse_ino_t ino, unsigned int cmd, void *arg, struct fuse_file_info *fi, unsigned flags, const void *in_buf, size_t in_bufsz, size_t out_bufsz);
typedef void (*hobo_poll_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph);
typedef void (*hobo_write_buf_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv, off_t off, struct fuse_file_info *fi);
typedef void (*hobo_retrieve_reply_t) (fuse_req_t req, void *cookie, fuse_ino_t ino, off_t offset, struct fuse_bufvec *bufv);
typedef void (*hobo_forget_multi_t) (fuse_req_t req, size_t count, struct fuse_forget_data *forgets);
typedef void (*hobo_flock_t) (fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, int op);
typedef void (*hobo_fallocate_t) (fuse_req_t req, fuse_ino_t ino, int mode, off_t offset, off_t length, struct fuse_file_info *fi);
typedef void (*hobo_readdirplus_t) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
typedef void (*hobo_copy_file_range_t) (fuse_req_t req, fuse_ino_t ino_in, off_t off_in, struct fuse_file_info *fi_in, fuse_ino_t ino_out, off_t off_out, struct fuse_file_info *fi_out, size_t len, int flags);

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

extern hobo_object_t *root_hob;

#endif // __HOBO_H__
