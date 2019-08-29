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
    hobo_object_t *hob = NULL;
    int code = 0;
    char *buffer = NULL;
    size_t length = 0;
    int responded = 0;

    fprintf(stderr, "HoboFS: called %s, req = 0x%p, ino = %lu, size = %zu, off = %zu, fi = 0x%p\n",__PRETTY_FUNCTION__, req, ino, size, off, fi);

    while (NULL == hob) {
        hob = hobo_object_lookup_by_ino(ino);
        if (NULL == hob) {
            code = EINVAL;
            break;
        }
        assert((uintptr_t)hob == fi->fh); // logic invariant

        buffer = (char *)malloc(size);
        if (NULL == buffer) {
            code = ENOMEM;
            break;
        }

        // TODO: generalize packing the directory entries
        // TODO: deal with the iterative impact and index values - this is a hack for now

        if (off > 1) { // this means we have no more entries to return
            code = fuse_reply_buf(req, NULL, 0);
            responded = 1;
            break;
        }

        length += fuse_add_direntry(req, buffer+length, size - length, ".", (struct stat *)hob->data, 1);
        if (length > size) {
            // buffer is too small
            code = fuse_reply_buf(req, NULL, 0);
            responded = 1;
            break;
        }
        length += fuse_add_direntry(req, buffer+length, size - length, "..", (struct stat *)hob->data, 2);
        if (length > size) {
            // buffer is too small
            code = fuse_reply_buf(req, NULL, 0);
            responded = 1;
            break;
        }

        /*
         * at this point I'm done
         */
        code = fuse_reply_buf(req, buffer, length);
        responded = 1;
        break;
    }

    assert(responded);

    if (NULL != buffer) {
        free(buffer);
        buffer = NULL;
    }

    if (0 > code) {
        fuse_reply_err(req, code);
    }

}

hobo_readdir_t hobo_readdir = readdir;
