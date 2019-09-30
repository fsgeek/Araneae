//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#pragma once

#if !defined(__HOBODB_H__)
#define __HOBODB_H__ (1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "hobodb-types.h"
#include "hobo-list.h"
#include <uuid/uuid.h>
#include <whitedb/dbapi.h>
#include <whitedb/indexapi.h>

typedef struct {
    void *record; // if not NULL, this is a database record
    uuid_t type;  // Type - type of this record (as a UUID)
    uuid_t uuid; // UUID of the record
    uint64_t ctime;
    uint64_t atime;
    struct {
        char *prefix;
        char *name;
    } uri;
} hobodb_base_t;

typedef struct {
    hobodb_base_t;
    uuid_t object1; // first object of the relationship - immutable
    uuid_t object2; // second object of the relationship - immutable
    uuid_t relationship; // this is the relationship type - immutable
    uuid_t properties; // any properties of this specific relationship - mutable
    uuid_t attributes; // any attributes of this specific relationship - mutable
    uuid_t labels; // any labels of this specpfic relationship - mutable
} hobodb_relationship_t;


typedef struct {
    hobodb_base_t;
    char *name;
    void *value;
    size_t length;
} hobodb_properties_t;

typedef struct {
    hobodb_base_t;
    uuid_t attribute_uuid;
    uuid_t next;
    void *value;
    size_t length;
} hobodb_attributes_t;

typedef struct {
    hobodb_base_t;
    uuid_t next;
    char *name;
} hobodb_label_t;


// This API is in flux... 28 August 2019 (WAM)
// TODO: use one format for these routines either hobodb_verb_noun or hobodb_noun_verb, not both
//
int hobodb_base_encode(void *db, hobodb_base_t *base); // might want to hide this
int hobodb_base_decode(void *db, hobodb_base_t *base); // might want to hide this
hobodb_base_t *hobodb_alloc_base(void);
void hobodb_free_base(hobodb_base_t *base);
hobodb_base_t *hobodb_lookup_base(void *db, uuid_t uuid);
int hobodb_update_base(void *db, hobodb_base_t *base);


int hobodb_relationship_encode(void *db, hobodb_relationship_t *relationship);
int hobodb_relationship_decode(void *db, hobodb_relationship_t *relationship);
hobodb_relationship_t *hobodb_alloc_relationship(void);
void hobodb_free_relationship(hobodb_relationship_t *relationship);
int hobodb_update_relationship(void *db, hobodb_relationship_t *relationship);

int hobodb_properties_encode(void *db, hobodb_properties_t *properties);
int hobodb_properties_decode(void *db, void *record, hobodb_properties_t *properties);
hobodb_properties_t *hobodb_alloc_properties(unsigned count);
void hobodb_free_properties(void *properties);
int hobodb_update_properties(void *db, hobodb_properties_t *properties);


int hobodb_attributes_encode(void *db, hobodb_attributes_t *attributes);
int hobodb_attributes_decode(void *db, void *record, hobodb_attributes_t *attributes);
hobodb_attributes_t *hobodb_alloc_attributes(unsigned count);
void hobodb_free_attributes(hobodb_attributes_t *attributes);
int hobodb_update_attributes(void *db, hobodb_attributes_t *attributes);


int hobodb_labels_encode(void *db, hobodb_label_t *labels);
int hobodb_labels_decode(void *db, void *record, hobodb_label_t *labels);
hobodb_label_t *hobodb_alloc_label(unsigned count);
void hobodb_free_label(hobodb_label_t *labels);
int hobodb_update_label(void *db, hobodb_label_t *label);

void *hobodb_lookup_object(void *db, const uuid_t uuid);
void *hobodb_lookup_relationship(void *db, const uuid_t object);
hobodb_relationship_t *hobodb_lookup_relationship_next(void *list);


void hobo_register_relationship(uuid_t relationship_uuid, const char *relationship_name);
void hobo_lookup_relationship_by_name(const char *relationship_name, uuid_t *relationship_uuid);
void hobo_lookup_relationship_by_uuid(uuid_t relationship_uuid, char **relationship_name);

const char *hobodb_lookup_record_type_uuid_string(const char *name);
const char *hobodb_lookup_record_type_name(const uuid_t uuid);
void hobodb_lookup_record_type_uuid(const char *name, uuid_t uuid);


#endif // __HOBODB_H__