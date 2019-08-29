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
    hobo_object_t *hob;

    fprintf(stderr, "HoboFS: called %s, req = 0x%p, ino = %lu, fi = 0x%p\n",__PRETTY_FUNCTION__, req, ino, fi);

    hob = hobo_object_lookup_by_ino(ino);
    if (NULL == hob) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if (NULL == hob->data) {
        fuse_reply_err(req, ENOTSUP);
        return;
    }

    fuse_reply_attr(req, hob->data, 86400); // cache for a day
    return;
}

hobo_getattr_t hobo_getattr = getattr;
