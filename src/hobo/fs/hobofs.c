//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#define FUSE_USE_VERSION 34

#include <fuse_lowlevel.h>
#include <hobofs.h>
#include <hobo.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc > 1) {
        fprintf(stderr, "unknown options @ %p\n", argv);
    }
    return (0);
}