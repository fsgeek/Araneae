//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void haccess(fuse_req_t req, fuse_ino_t ino, int mask)
{
    fprintf(stderr, "HoboFS: called %s, req = 0x%p, ino = %lu, mask = 0x%u\n",__PRETTY_FUNCTION__, req, ino, (unsigned) mask);

    // for now we always grant access.
    fuse_reply_err(req, 0);
}

hobo_access_t hobo_access = haccess;
