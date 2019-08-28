
#include "hobo.h"
#include <pthread.h>

static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    hobo_object_t *hob;
    
    fprintf(stderr, "HoboFS: called %s, req = 0x%p, parent = %lu, name = %s\n",__PRETTY_FUNCTION__, req, parent, name);

    hob = hobo_object_lookup_by_ino(parent);

    if (NULL == hob) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    // TODO: stub in the lookup logic here
    //  - if the name is a UUID, we look up the object independent of the parent (don't care)
    //  - if the name is a string, we lookup the object based upon the parent (container) relationship
    fuse_reply_err(req, ENOENT);
}

hobo_lookup_t hobo_lookup = lookup;
