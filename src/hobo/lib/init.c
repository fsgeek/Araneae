//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

hobo_object_t *root_hob = NULL;

static struct stat root_statbuf;

static void init(void *userdata, struct fuse_conn_info *conn)
{
    static int initialized = 0;

    fprintf(stderr, "HoboFS: called %s, userdata = 0x%p, conn = 0x%p\n",__PRETTY_FUNCTION__, userdata, conn);

    // TODO: initialization

    while (!initialized) {
        // initialization logic goes here

        root_statbuf.st_ino = FUSE_ROOT_ID;
        root_statbuf.st_mode = S_IFDIR | 0777;
        root_statbuf.st_nlink = 1;
        root_statbuf.st_uid = getuid();
        root_statbuf.st_gid = getgid();
        root_statbuf.st_size = 0x1000;
        root_statbuf.st_blocks = 1;
        root_statbuf.st_atime = time(NULL);
        root_statbuf.st_mtime = root_statbuf.st_atime;
        root_statbuf.st_ctime = root_statbuf.st_ctime;

        root_hob = hobo_object_create(FUSE_ROOT_ID, NULL, &root_statbuf);
        assert(NULL != root_hob);

        // TODO: this is where I'd want to load in any initial state from the persistent store

        //
        // Done
        //
        initialized = 1;
    }
}

hobo_init_t hobo_init = init;
