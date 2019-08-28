//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#define FUSE_USE_VERSION 36

#include <fuse_lowlevel.h>

extern struct fuse_lowlevel_ops hobo_ops;