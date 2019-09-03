//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void hmkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
{
    hobo_object_t *pob = NULL; 

    fprintf(stderr, "HoboFS: called %s, req = 0x%p, parent = %lu, name = %s, mode = 0x%x\n", __PRETTY_FUNCTION__, req, parent, name, mode);

    pob = hobo_object_lookup_by_ino(parent);
    if (NULL == pob) {
        fuse_reply_err(req, ENOENT);
        return;
    }
    fuse_reply_err(req, ENOTSUP);
    return;
}


hobo_mkdir_t hobo_mkdir = hmkdir;







