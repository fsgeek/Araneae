//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#pragma once

#if !defined(__HOBO_H__)
#define __HOBO_H__ (1)

#include "hobofs.h"
#include "ino-lookup.h"

typedef void (*hobo_lookup_t)(fuse_req_t req, fuse_ino_t parent, const char *name);
extern hobo_lookup_t hobo_lookup;

typedef void (*hobo_getattr_t)(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
extern hobo_getattr_t hobo_getattr;


#endif // __HOBO_H__
