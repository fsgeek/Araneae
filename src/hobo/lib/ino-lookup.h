//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#if !defined(__HOBO_LOOKUP_H__)
#define __HOBO_LOOKUP_H__ (1)

#include "hobofs.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fuse_lowlevel.h>


#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS (64)
#endif // _FILE_OFFSET_BITS

#include <fuse_lowlevel.h>
#include <uuid/uuid.h>
#include <stdint.h>
#include "hobo-list.h"

//
// Hobo must be able to look up by inode number and UUID (the two sources of handles)
//
typedef struct _hobo_object
{
    fuse_ino_t inode;
    uuid_t uuid;
    // TODO: we may need additional data here
    void *data; // externally provided data
} hobo_object_t;

typedef void (*hobo_object_cleanup_callback_t)(void *);

hobo_object_t *hobo_object_lookup_by_ino(fuse_ino_t inode);
hobo_object_t *hobo_object_lookup_by_uuid(uuid_t *uuid);
void hobo_object_release(hobo_object_t *object, hobo_object_cleanup_callback_t cleanup);
hobo_object_t *hobo_object_create(fuse_ino_t inode, uuid_t *uuid, void *data);

#endif // __HOBO_LOOKUP_H__


