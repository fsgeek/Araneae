//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    (void)fi; // unreferenced
    (void)ino;

    // stub
    fuse_reply_err(req, ENOTSUP);
}

hobo_getattr_t hobo_getattr = getattr;
