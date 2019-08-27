#define FUSE_USE_VERSION 34

#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>



static void hobo_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    struct fuse_entry_param e;

    if (1 == parent) {
        // handle root here
    }

    // stub
    fuse_reply_err(req, ENOTSUP);
}

static void hobo_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    (void)fi; // unreferenced
    (void)ino;

    // stub
    fuse_reply_err(req, ENOTSUP);
}

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

static struct fuse_lowlevel hobo_ops = {
    .lookup = hobo_lookup,
    .getattr = hobo_getattr,
    .readdir = hobo_readdir,
    .open = hobo_open,
};