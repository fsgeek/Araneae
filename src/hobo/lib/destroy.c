//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"
#include <whitedb/dbapi.h>

static void destroy(void *userdata)
{
    fprintf(stderr, "HoboFS: called %s, userdata = 0x%p\n",__PRETTY_FUNCTION__, userdata);

    if (NULL != root_hob) {
        hobo_object_release(root_hob, NULL); // don't need a callback in this case because our root object had a static structure
        root_hob = NULL;
    }

    // TODO: cleanup/save any persistent store
    wg_detach_database(hobo_db);

}
hobo_destroy_t hobo_destroy = destroy;
