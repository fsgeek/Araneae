//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobodb.h"

const int hobodb_base_field_type = 0;
const int hobodb_base_field_uuid = 1;
const int hobodb_base_field_ctime = 2;
const int hobodb_base_field_atime = 3;
const int hobodb_base_field_uri = 4;
const int hobodb_base_field_max = 5; // marks the end of the base fields

const int hobodb_relationship_field_object1 = 5;
const int hobodb_relationship_field_object2 = 6;
const int hobodb_relationship_field_relationship = 7;
const int hobodb_relationship_field_properties = 8;
const int hobodb_relationship_field_attributes = 9;
const int hobodb_relationship_field_labels = 10;
const int hobodb_relationship_field_max = 11;

// TODO: define field locations for other object types


// forward reference
static void record_type_to_uuid(const char *type, uuid_t uuid);


int hobodb_base_encode(void *db, hobodb_base_t *base, void **record)
{
    void *rec;
    wg_int enc;
    wg_int lock_id;
    uuid_t base_type_uuid;
    char uuid_type[16];
    
    
    record_type_to_uuid("base", base_type_uuid);
    assert(!uuid_is_null(base_type_uuid));
    assert(NULL != base);
    assert(0 == uuid_compare(base_type_uuid, base->type));
    assert(NULL != record); // not valid
    assert(NULL == *record); // shouldn't be passing it in, right?
    memset(uuid_type, 0, sizeof(uuid_type));
    strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);

    if (NULL == base->record) {
        base->record = wg_create_record(db, hobodb_base_field_max);
        if (NULL == base->record) {
            return -ENOMEM;
        }
        rec = base->record;

        lock_id = wg_start_write(db);
        while (0 != lock_id) {
            // store type field
            enc = wg_encode_blob(db, (char *)base->type, uuid_type, (wg_int)sizeof(uuid_t));
            assert(WG_ILLEGAL != enc);
            assert(wg_set_field(db, rec, hobodb_base_field_type, enc) >= 0);
            
            // store uuid of this record
            enc = wg_encode_blob(db, (char *)base->uuid, uuid_type, (wg_int)sizeof(uuid_t));
            assert(WG_ILLEGAL != enc);
            assert(wg_set_field(db, rec, hobodb_base_field_uuid, enc) >= 0);

            // TODO: should we store these as database timestamps?
            // store ctime of this record
            enc = wg_encode_int(db, base->ctime);
            assert(WG_ILLEGAL != enc);
            assert(wg_set_field(db, rec, hobodb_base_field_ctime, enc) >= 0);

            enc = wg_encode_int(db, base->atime);
            assert(WG_ILLEGAL != enc);
            assert(wg_set_field(db, rec, hobodb_base_field_atime, enc) >= 0);

            enc = wg_encode_uri(db, base->uri.name, base->uri.prefix);
            assert(WG_ILLEGAL != enc);
            assert(wg_set_field(db, rec, hobodb_base_field_uri, enc) >= 0);
            
            assert(0 != wg_end_write(db, lock_id)); // unlock failure is catastrophic
        }
    }

    return 0;
}

int hobodb_base_decode(void *db, void *record, hobodb_base_t *base)
{
    uuid_t base_type_uuid;
    wg_int length;
    wg_int lock_id;
    wg_int enc;

    assert(NULL != db);
    assert(NULL != base);
    assert(NULL != record);
    assert(NULL == base->record); // assume we aren't replacing the record - if we are, more code to write!

    record_type_to_uuid("base", base_type_uuid);
    assert(!uuid_is_null(base_type_uuid));

    lock_id = wg_start_read(db);
    while (0 != lock_id) {
        length = wg_get_record_len(db, record);
        assert(length >= hobodb_base_field_max);

        enc = wg_get_field(db, record, hobodb_base_field_type);
        assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc));
        assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc));
        memcpy(&base->type, wg_decode_blob(db, enc), sizeof(uuid_t));

        enc = wg_get_field(db, record, hobodb_base_field_uuid);
        assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc));
        assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc));
        memcpy(&base->uuid, wg_decode_blob(db, enc), sizeof(uuid_t));

        enc = wg_get_field(db, record, hobodb_base_field_ctime);
        assert(WG_INTTYPE == wg_get_encoded_type(db, enc));
        base->ctime = wg_decode_int(db, enc);

        enc = wg_get_field(db, record, hobodb_base_field_atime);
        assert(WG_INTTYPE == wg_get_encoded_type(db, enc));
        base->atime = wg_decode_int(db, enc);

        enc = wg_get_field(db, record, hobodb_base_field_uri);
        assert(WG_URITYPE == wg_get_encoded_type(db, enc));
        length = wg_decode_uri_len(db, enc);
        base->uri.name = malloc(length + sizeof(char));
        wg_decode_uri_copy(db, enc, base->uri.name, length + sizeof(char));
        length = wg_decode_uri_prefix_len(db, enc);
        if (length > 0) {
            base->uri.prefix = malloc(length + sizeof(char));
            wg_decode_uri_copy(db, enc, base->uri.name, length + sizeof(char));
        }
        else {
            base->uri.prefix = NULL;
        }

        assert(0 != wg_end_read(db, lock_id)); // unlock failure is catastrophic
    }

    return 0;
}

hobodb_base_t *hobodb_alloc_base(void)
{
    hobodb_base_t *newbase = (hobodb_base_t *)malloc(sizeof(hobodb_base_t));

    while (NULL != newbase) {
        memset(newbase, 0, sizeof(hobodb_base_t));
    }

    return newbase;
}

void hobodb_free_base(hobodb_base_t *base)
{
    if (NULL != base) {
        free(base);
        base = NULL;
    }
}

hobodb_base_t *hobodb_lookup_base(void *db, uuid_t uuid)
{
    (void) db;
    (void) uuid;

    return (hobodb_base_t *)0;
}

int hobodb_update_base(void *db, hobodb_base_t *base)
{
    (void) db;
    (void) base;

    return -ENOTSUP;
}






int hobodb_relationship_encode(void *db, hobodb_relationship_t *relationship, void **record)
{
    (void) db;
    (void) relationship;
    (void) record;
    if (NULL != record) {
        *record = NULL;
    }
    return ENOTSUP;
    
}


int hobodb_relationship_decode(void *db, void *record, hobodb_relationship_t *relationship)
{
    (void) db;
    (void) relationship;
    (void) record;
    return ENOTSUP;

}

int hobodb_properties_encode(void *db, hobodb_properties_t *properties, void **record)
{
    (void) db;
    (void) properties;
    (void) record;
    if (NULL != record) {
        *record = NULL;
    }
    return ENOTSUP;
}



int hobodb_properties_decode(void *db, void *record, hobodb_properties_t *properties)
{
    (void) db;
    (void) properties;
    (void) record;
    return ENOTSUP;
}

int hobodb_attributes_encode(void *db, hobodb_attributes_t *attributes, void **record)
{
    (void) db;
    (void) attributes;
    (void) record;
    if (NULL != record) {
        *record = NULL;
    }
    return ENOTSUP;
}

int hobodb_attributes_decode(void *db, void *record, hobodb_attributes_t *attributes)
{
    (void) db;
    (void) attributes;
    (void) record;
    return ENOTSUP;
}

int hobodb_labels_encode(void *db, hobodb_label_t *labels, void **record)
{
    (void) db;
    (void) labels;
    (void) record;
    if (NULL != record) {
        *record = NULL;
    }
    return ENOTSUP;
}

int hobodb_labels_decode(void *db, void *record, hobodb_label_t *labels)
{
    (void) db;
    (void) labels;
    (void) record;
    return ENOTSUP;
}
/*
    hobodb_base_object_type = 100,
    hobdb_relationship_object_type = 200,
    hobodb_property_object_type = 300,
    hobodb_attribute_object_type = 400,
    hobodb_label_object_type = 500,
*/

typedef struct {
    const char *uuid;
    const char *name;
    hobodb_object_types_t type;
} hobo_record_type_t;

static hobo_record_type_t hobo_record_type_uuids[] = {
    {"407d2137-7b37-42eb-aa95-049c35a61dd1", "base", hobodb_base_object_type},
    {"228dee9c-7992-4c1a-bcbd-e7c99e8640f3", "relationship", hobdb_relationship_object_type},
    {"3379e377-5a1a-4810-88de-b6e65d97ee19", "property", hobodb_property_object_type},
    {"2da1ebd3-6098-4cfd-8480-edf43babfafd", "attribute", hobodb_attribute_object_type},
    {"06d119e4-5893-40ea-9565-313b772b8573", "label", hobodb_label_object_type},
};

static void record_type_to_uuid(const char *type, uuid_t uuid)
{
    uuid_clear(uuid);
    for (unsigned index = 0; index < (sizeof(hobo_record_type_uuids)/sizeof(hobo_record_type_t)); index++) {
        if (0 == strcmp(hobo_record_type_uuids[index].name, type)) {
            assert(0 == uuid_parse(hobo_record_type_uuids[index].uuid, uuid));
            break;
        }
    }
    return;
}