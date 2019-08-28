//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi)
{
    fprintf(stderr, "HoboFS: called %s, req = 0x%p, ino = %lu, size = %zu, off = %zu, fi = 0x%p\n",__PRETTY_FUNCTION__, req, ino, size, off, fi);

    fuse_reply_err(req, ENOTSUP);
}

hobo_readdir_t hobo_readdir = readdir;
