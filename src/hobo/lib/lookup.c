
#include "hobo.h"
#include <pthread.h>

static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    hobo_object_t *hob = hobo_object_lookup_by_ino(parent);

    if (NULL == hob) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    (void) name;

    // TODO: stub in the lookup logic here
    //  - if the name is a UUID, we look up the object independent of the parent (don't care)
    //  - if the name is a string, we lookup the object based upon the parent (container) relationship
    fuse_reply_err(req, ENOENT);
}

static void lookup_init(fuse_req_t req, fuse_ino_t parent, const char *name);
hobo_lookup_t hobo_lookup = lookup_init;

static void lookup_init(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    static int initialized = 0;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&lock);
    while (!initialized) {
        // initialization logic goes here
        assert ((fuse_ino_t) FUSE_ROOT_ID == parent);
        hobo_object_t *hob = hobo_object_create(parent, NULL);
        assert(NULL != hob);

        // TODO: this is where I'd want to load in any initial state from a persistent store

        //
        // Done
        //
        hobo_lookup = lookup;
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        initialized = 1;
    }
    pthread_mutex_unlock(&lock);

    lookup(req, parent, name);
}
