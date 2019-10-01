//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#if !defined(__HOBODB_UTIL_H__)
#define __HOBODB_UTIL_H__ (1)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <pthread.h>
#include "hobodb.h"

#if !defined(__notused)
#define __notused __attribute__((unused))
#endif //

typedef struct _hobo_util_create_object_parameters_t {
    hobo_object_types_t type;
    uuid_t uuid; // clear = generate, !clear = use
    union {
        struct {
            struct {
                const char *prefix; // optional URI prefix
                const char *name; // URI name (required)
            } uri;
        } base_object_parameters;

        struct {
            uuid_t object1;
            uuid_t object2;
            uuid_t relationship;
        } relationship_object_parameters;
        struct {
            uuid_t object;
            uuid_t property;
            size_t length;
            void *value;
        } property_object_parameters;
        struct {
            uuid_t object;
            uuid_t property;
        } attribute_object_parameters;
        struct {
            uuid_t object;
            uuid_t uuid;
            const char *label;
        } label_object_parameters;
    } parameters;
} hobo_util_create_parameters_t;

hobodb_base_t *hobo_util_create_object(hobo_util_create_parameters_t *parameters);
int hobo_util_update_object(void *db, hobodb_base_t *object);
int hobo_util_encode_object(void *db, hobodb_base_t *object);
hobodb_base_t *hobo_util_lookup_object(void *db, const uuid_t uuid);


#endif // !defined(__HOBODB_UTIL_H__)
