//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    hobo_object_t *hob = NULL;

    fprintf(stderr, "HoboFS: called %s, req = 0x%p, ino = %lu, fi = 0x%p\n",__PRETTY_FUNCTION__, req, ino, fi);

    hob = hobo_object_lookup_by_ino(ino);
    if (NULL == hob) {
        fuse_reply_err(req, EINVAL);
        return;
    }
    assert(fi->fh == (uintptr_t)hob); // they need to be the same!
    hobo_object_release(hob, NULL);
    hobo_object_release((hobo_object_t *)fi->fh, NULL); // at the current time we don't allocate anything, so no need to delete it
    fuse_reply_err(req, 0); // success!
    return;
}

hobo_releasedir_t hobo_releasedir = releasedir;



