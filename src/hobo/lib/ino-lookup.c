//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ino-lookup.h"

#if !defined(offset_of)
#define offset_of(type, field) (unsigned long)&(((type *)0)->field)
#endif // offset_of

#if !defined(container_of)
#define container_of(ptr, type, member) ((type *)(((char *)ptr) - offset_of(type, member)))
#endif // container_of

//
// The current implementation is a simple linked list.  If this isn't sufficiently efficient,
// the interface is general and it can be moved to a more efficient structure.
//
// A reader/writer lock is used to protect.  The read lock is used to protect against insertion/deletion
// and the write lock is used to allow insertion/deletion.
//
// Each object has a reference count; when the reference count goes to zero, it can be removed from
// the list IFF the list lock is held exclusive.
//
static void lock_table_for_lookup(void);
static void lock_table_for_change(void);
static void unlock_table(void);

list_entry_t table_list = {.next = &table_list, .prev = &table_list};

typedef struct _hobo_internal_object
{
    list_entry_t list_entry;
    uint32_t refcount;
    hobo_object_t object;
} hobo_internal_object_t;

static hobo_internal_object_t *lookup_by_ino_locked(fuse_ino_t inode)
{
    hobo_internal_object_t *internal_object;
    list_entry_t *entry;
    int found = 0;

    list_for_each(&table_list, entry)
    {
        internal_object = container_of(entry, hobo_internal_object_t, list_entry);
        if (internal_object->object.inode == inode)
        {
            found = 1;
            __atomic_fetch_add(&internal_object->refcount, 1, __ATOMIC_RELAXED);
            break;
        }
    }

    return found ? internal_object : NULL;
}

static hobo_internal_object_t *lookup_by_uuid_locked(uuid_t *uuid)
{
    hobo_internal_object_t *internal_object;
    list_entry_t *entry;
    int found = 0;

    list_for_each(&table_list, entry)
    {
        internal_object = container_of(entry, hobo_internal_object_t, list_entry);
        if (0 == memcmp(&internal_object->object.uuid, uuid, sizeof(uuid_t)))
        {
            found = 1;
            __atomic_fetch_add(&internal_object->refcount, 1, __ATOMIC_RELAXED);
            break;
        }
    }

    return found ? internal_object : NULL;
}

static void release(hobo_internal_object_t *object, hobo_object_cleanup_callback_t cleanup)
{
    void *cleanup_data = NULL;
    //
    // this is the only place I try to be tricky
    // To delete, the refcount must do the 1->0 transition
    // but since most transitions will be to > 0 I don't want to take the lock
    // exclusive.  So I hold the lock shared (preventing deletion) and decrement
    // and see if this might be a deletion call.  If it _might_ be then I have to
    // bump the refcount and try again with the exclusive lock held.  If it
    //  *still* drops to zero, I can safely remove and delete the object as nobody
    // else can find it.
    //
    // Lookup: hold lock shared, atomic increment
    // Release: hold lock shared, atomic decrement IFF decrement causes deletion, atomic increment
    //          THEN hold lock exclusive, atomic decrement IFF decrement causes deletion, remove and free.
    //
    //
    assert(object->list_entry.next != &object->list_entry);
    lock_table_for_lookup();
    if (1 == __atomic_fetch_sub(&object->refcount, 1, __ATOMIC_RELAXED))
    {
        // this is an actual delete, so we need the lock exclusive
        __atomic_fetch_add(&object->refcount, 1, __ATOMIC_RELAXED);
        unlock_table();
        lock_table_for_change();
        if (1 == __atomic_fetch_sub(&object->refcount, 1, __ATOMIC_RELAXED))
        {
            // now it's safe to remove it
            remove_list_entry(&object->list_entry);
            cleanup_data = object->object.data;
            free(object);
        }
    }
    unlock_table();

    if ((NULL != cleanup) && (NULL != cleanup_data)) {
        cleanup(cleanup_data);
    }
}

static hobo_internal_object_t *object_create(fuse_ino_t inode, uuid_t *uuid, void *data)
{
    hobo_internal_object_t *internal_object = (hobo_internal_object_t *)malloc(sizeof(hobo_internal_object_t));
    hobo_internal_object_t *dummy = NULL;

    while (NULL != internal_object)
    {
        initialize_list_entry(&internal_object->list_entry);
        memcpy(internal_object->object.uuid, uuid, sizeof(uuid_t));
        internal_object->object.inode = inode;
        internal_object->object.data = data;
        internal_object->refcount = 2;

        lock_table_for_change();
        dummy = lookup_by_ino_locked(inode);
        // assert(NULL == dummy); // shouldn't be one
        if (NULL == dummy)
        {
            insert_list_head(&table_list, &internal_object->list_entry);
            unlock_table();
        }
        else
        {
            unlock_table();
            free(internal_object);
            internal_object = dummy;
            dummy = NULL;
        }
        break;
    }
    assert(NULL == dummy);
    return internal_object;
}

hobo_object_t *hobo_object_create(fuse_ino_t inode, uuid_t *uuid, void *data)
{
    uuid_t dummy_uuid;

    if (NULL == uuid)
    {
        uuid_generate(dummy_uuid);
        uuid = &dummy_uuid;
    }

    hobo_internal_object_t *internal_object = object_create(inode, uuid, data);

    return internal_object ? &internal_object->object : NULL;
}

hobo_object_t *hobo_object_lookup_by_ino(fuse_ino_t inode)
{
    hobo_internal_object_t *internal_object;

    lock_table_for_lookup();
    internal_object = lookup_by_ino_locked(inode);
    unlock_table();
    return internal_object ? &internal_object->object : NULL;
}

hobo_object_t *hobo_object_lookup_by_uuid(uuid_t *uuid)
{
    hobo_internal_object_t *internal_object;

    lock_table_for_lookup();
    internal_object = lookup_by_uuid_locked(uuid);
    unlock_table();
    return internal_object ? &internal_object->object : NULL;
}

void hobo_object_release(hobo_object_t *object, hobo_object_cleanup_callback_t cleanup)
{
    hobo_internal_object_t *internal_object = container_of(object, hobo_internal_object_t, object);

    release(internal_object, cleanup);
}

static pthread_rwlock_t table_lock = PTHREAD_RWLOCK_INITIALIZER;

static void lock_table_for_lookup(void)
{
    pthread_rwlock_rdlock(&table_lock);
}

static void lock_table_for_change(void)
{
    pthread_rwlock_wrlock(&table_lock);
}

static void unlock_table(void)
{
    pthread_rwlock_unlock(&table_lock);
}

