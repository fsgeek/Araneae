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


static void hobo_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi)
{
    (void) size;
    (void) off;
    (void) fi;
    (void) ino;

    // stub
    fuse_reply_err(req, ENOTSUP);
}

static void hobo_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    (void) fi;
    (void) ino;

    fuse_reply_err(req, ENOTSUP);
}

// registration structure for operations we support

static struct fuse_lowlevel_ops hobo_ops;

const struct fuse_lowlevel_ops *hobo_init(void)
{
    hobo_ops.lookup = hobo_lookup;
    hobo_ops.getattr = hobo_getattr;
    hobo_ops.readdir = hobo_readdir;
    hobo_ops.open = hobo_open;

    return &hobo_ops;
}
