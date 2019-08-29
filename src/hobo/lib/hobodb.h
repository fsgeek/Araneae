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
#include "hobo.h"
#include <whitedb/dbapi.h>
#include <whitedb/indexapi.h>


typedef enum {
    hobodb_base_object_type = 100,
    hobdb_relationship_object_type = 200,
    hobodb_property_object_type = 300,
    hobodb_attribute_object_type = 400,
    hobodb_label_object_type = 500,
} hobodb_object_types_t;

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
    uuid_t object1; // first object of the relationship
    uuid_t object2; // second object of the relationship
    uuid_t relationship; // this is the relationship type
    uuid_t properties; // any properties of this specific relationship
    uuid_t attributes; // any attributes of this specific relationship
    uuid_t labels; // any labels of this specpfic relationship
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
int hobodb_base_encode(void *db, hobodb_base_t *base, void **record); // might want to hide this
int hobodb_base_decode(void *db, void *record, hobodb_base_t *base); // might want to hide this
hobodb_base_t *hobodb_alloc_base(void);
void hobodb_free_base(hobodb_base_t *base);
hobodb_base_t *hobodb_lookup_base(void *db, uuid_t uuid);
int hobodb_update_base(void *db, hobodb_base_t *base);


int hobodb_relationship_encode(void *db, hobodb_relationship_t *relationship, void **record);
int hobodb_relationship_decode(void *db, void *record, hobodb_relationship_t *relationship);
int hobodb_allocate_relationship(void *db, hobodb_relationship_t **relationship);
int hobodb_free_relationship(void *db, hobodb_relationship_t *relationship);

int hobodb_properties_encode(void *db, hobodb_properties_t *properties, void **record);
int hobodb_properties_decode(void *db, void *record, hobodb_properties_t *properties);

int hobodb_attributes_encode(void *db, hobodb_attributes_t *attributes, void **record);
int hobodb_attributes_decode(void *db, void *record, hobodb_attributes_t *attributes);

int hobodb_labels_encode(void *db, hobodb_label_t *labels, void **record);
int hobodb_labels_decode(void *db, void *record, hobodb_label_t *labels);

int hobodb_lookup_object(void *db, uuid_t uuid);


#endif // __HOBODB_H__