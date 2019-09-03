//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobodb.h"

#if !defined(offset_of)
#define offset_of(type, field) (unsigned long)&(((type *)0)->field)
#endif // offset_of

#if !defined(container_of)
#define container_of(ptr, type, member) ((type *)(((char *)ptr) - offset_of(type, member)))
#endif // container_of

#if !defined(TRUE)
#define TRUE (1)
#endif // TRUE


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


// helper function
static wg_int encode_uuid_as_string(void *db, uuid_t uuid)
{
    char uuid_string[40];
    wg_int enc;

    assert(NULL != db);
    assert(!uuid_is_null(uuid)); // don't use the NULL uuid

    uuid_unparse_lower(uuid, uuid_string);

    enc = wg_encode_str(db, uuid_string, NULL);
    assert(WG_ILLEGAL != enc);

    return enc;    
}

static void decode_uuid_from_string(void *db, wg_int enc, uuid_t uuid)
{
    char *uuid_string = NULL;

    uuid_string = (char *)malloc(40); // enough space for the UUID as a string

    while (NULL != uuid_string) {
        assert(wg_decode_str_copy(db, enc, uuid_string, 40) >= 0);
        break;
    }

    assert(36 == strlen(uuid_string));
    assert(0 == uuid_parse(uuid_string, uuid));
}


static void init_base(hobodb_base_t *base) 
{
    record_type_to_uuid("base", base->type);
    uuid_generate(base->uuid);
    assert(!uuid_is_null(base->uuid));

    base->ctime = time(NULL);
    base->atime = time(NULL);
}

//
// This encodes the base fields; if it is the initial pass,
// we will set the type and uuid.  Once set, we treat those as
// invariant.
//
static int generic_base_encode(void *db, hobodb_base_t *base, int initial)
{
    wg_int lock_id;
    wg_int enc_type;
    wg_int enc_uuid;
    wg_int enc_ctime;
    wg_int enc_atime;
    wg_int enc_uri;
    int encoded = 0;
    char uuid_type[8];

    assert(NULL != db);
    assert(NULL != base);
    assert(NULL != base->record);

    assert(wg_get_record_len(db, base->record) >= hobodb_base_field_max);

    if (initial) {
        // must be null
        assert(0 == wg_get_field(db, base->record, hobodb_base_field_type));
        assert(0 == wg_get_field(db, base->record, hobodb_base_field_uuid));

        record_type_to_uuid("base", base->type);
        assert(!uuid_is_null(base->type));

        uuid_type[sizeof(uuid_type)-1] = '\0';
        strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);
        enc_type = wg_encode_blob(db, (char *)base->type, uuid_type, (wg_int)sizeof(uuid_t));
        assert(WG_ILLEGAL != enc_type);

        enc_uuid = encode_uuid_as_string(db, base->uuid);
        assert(WG_ILLEGAL != enc_uuid);

    }
    else {
        // MUST not be NULL - and must be correct type(s)
        assert(WG_BLOBTYPE == wg_get_field(db, base->record, hobodb_base_field_type));
        assert(WG_STRTYPE == wg_get_field(db, base->record, hobodb_base_field_uuid));
    } 

    enc_ctime = wg_encode_int(db, base->ctime);
    assert(WG_ILLEGAL != enc_ctime);

    enc_atime = wg_encode_int(db, base->atime);
    assert(WG_ILLEGAL != enc_atime);

    if (NULL != base->uri.name) {
        enc_uri = wg_encode_uri(db, base->uri.name, base->uri.prefix);
        assert(WG_ILLEGAL != enc_uri);
    }

    // Update under lock
    lock_id = wg_start_write(db);
    while (0 != lock_id) {
        if (initial) {
            // only write for initial creation
            assert(wg_set_field(db, base->record, hobodb_base_field_type, enc_type) >= 0);
            assert(wg_set_field(db, base->record, hobodb_base_field_uuid, enc_uuid) >= 0);
        }
        assert(wg_set_field(db, base->record, hobodb_base_field_ctime, enc_ctime) >= 0);
        assert(wg_set_field(db, base->record, hobodb_base_field_atime, enc_atime) >= 0);
        if (NULL != base->uri.name) {
            assert(wg_set_field(db, base->record, hobodb_base_field_uri, enc_uri) >= 0);
        }
        assert(0 != wg_end_write(db, lock_id)); // unlock failure is catastrophic
        encoded = 1;
        break;
    }

    assert(encoded);
    return 0;
}


int hobodb_base_encode(void *db, hobodb_base_t *base)
{
    uuid_t base_type_uuid;
    char uuid_type[16];
    int initial = 0;
    
    record_type_to_uuid("base", base_type_uuid);
    assert(!uuid_is_null(base_type_uuid));
    assert(NULL != base);
    assert(0 == uuid_compare(base_type_uuid, base->type));
    memset(uuid_type, 0, sizeof(uuid_type));
    strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);

    if (NULL == base->record) {
        base->record = wg_create_record(db, hobodb_base_field_max);
        if (NULL == base->record) {
            return -ENOMEM;
        }

        initial = TRUE;
    }
    generic_base_encode(db, base, initial);

    return 0;
}

//
// generic_base_decode: works on decoding the base portion of the object
// _without_ insisting it has to be an object of a specific type (leave
// that check to the caller!)
//
static int generic_base_decode(void *db, hobodb_base_t *base)
{
    wg_int lock_id;
    wg_int enc_type;
    wg_int enc_uuid;
    wg_int enc_ctime;
    wg_int enc_atime;
    wg_int enc_uri;
    int decoded = 0;
    wg_int length;

    assert(NULL != base);
    assert(NULL != base->record);

    assert(wg_get_record_len(db, base->record) >= hobodb_base_field_max);

    lock_id = wg_start_read(db);
    while (0 != lock_id) {
        enc_type = wg_get_field(db, base->record, hobodb_base_field_type);
        enc_uuid = wg_get_field(db, base->record, hobodb_base_field_uuid);
        enc_ctime = wg_get_field(db, base->record, hobodb_base_field_ctime);
        enc_atime = wg_get_field(db, base->record, hobodb_base_field_atime);
        enc_uri = wg_get_field(db, base->record, hobodb_base_field_uri);
        decoded = 1;
        assert(0 != wg_end_read(db, lock_id)); // unlock failure is catastrophic
        break;
    }
    assert(decoded); // this implies a lock failure, which is catastrophic for now

    assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc_type));
    assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc_type));
    memcpy(&base->type, wg_decode_blob(db, enc_type), sizeof(uuid_t));
    
    assert(WG_STRTYPE == wg_get_encoded_type(db, enc_uuid));
    decode_uuid_from_string(db, enc_uuid, base->uuid);

    assert(WG_INTTYPE == wg_get_encoded_type(db, enc_ctime));
    base->ctime = wg_decode_int(db, enc_ctime);

    assert(WG_INTTYPE == wg_get_encoded_type(db, enc_atime));
    base->atime = wg_decode_int(db, enc_atime);

    if (0 == enc_uri) { // no URI
        base->uri.name = NULL;
        base->uri.prefix  = NULL;
    }
    else {
        assert(WG_URITYPE == wg_get_encoded_type(db, enc_uri));
        length = wg_decode_uri_len(db, enc_uri) + sizeof(char);
        assert(length > sizeof(char)); // no empty strings allowed
        base->uri.name = malloc(length);
        wg_decode_uri_copy(db, enc_uri, base->uri.name, length);
        length = wg_decode_uri_prefix_len(db, enc_uri) + sizeof(char);
        if (length > 0) {
            base->uri.prefix = malloc(length);
            wg_decode_uri_prefix_copy(db, enc_uri, base->uri.prefix, length);
        }
        else {
            base->uri.prefix = NULL;
        }
    }
    return 0;

}

int hobodb_base_decode(void *db, hobodb_base_t *base)
{
    uuid_t base_type_uuid;

    assert(NULL != db);
    assert(NULL != base);
    assert(NULL != base->record); // we can't decode without a record

    record_type_to_uuid("base", base_type_uuid);
    assert(!uuid_is_null(base_type_uuid));

    assert(0 == generic_base_decode(db, base));

    return 0;
}


hobodb_base_t *hobodb_alloc_base(void)
{
    hobodb_base_t *newbase = (hobodb_base_t *)malloc(sizeof(hobodb_base_t));


    while (NULL != newbase) {
        memset(newbase, 0, sizeof(hobodb_base_t));
        init_base(newbase);
        // done
        break;
    }

    return newbase;
}

static void hobodb_cleanup_base(hobodb_base_t *base)
{
    if (NULL != base) {
        if (base->uri.name) {
            free(base->uri.name);
            base->uri.name = NULL;
        }
        if (base->uri.prefix) {
            free(base->uri.prefix);
            base->uri.prefix = NULL;
        }
    }

}

void hobodb_free_base(hobodb_base_t *base)
{
    if (NULL != base) {
        hobodb_cleanup_base(base);
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

//
// hobodb_update_base: we update the mutable fields of the base
//                     object.
//
// Note: the type and uuid of this object are immutable; we do not
//       overwrite them.
//
int hobodb_update_base(void *db, hobodb_base_t *base)
{
    wg_int enc_ctime = 0;
    wg_int enc_atime = 0;
    wg_int enc_uri = 0;
    int code = -EINVAL;
    wg_int lock_id;
    void *rec;

    // Check invariants
    assert(NULL != base);
    assert(NULL != base->record);
    rec = (void *) base->record;

    // encode ctime
    enc_ctime = wg_encode_int(db, base->ctime);
    assert(WG_ILLEGAL != enc_ctime);

    // encode atime
    enc_atime = wg_encode_int(db, base->atime);
    assert(WG_ILLEGAL != enc_atime);

    // encode uri
    enc_uri = wg_encode_uri(db, base->uri.name, base->uri.prefix);
    assert(WG_ILLEGAL != enc_uri);

    //
    // we update under the database lock
    // We encode without it, because we're changing local state
    // Of course, this doesn't guarantee someone else isn't
    // updating this already anyway.  LOTS of work to do to make
    // this code reasonably parallel (so this probably doesn't matter)
    //
    lock_id = wg_start_write(db);
    while (0 != lock_id) {
        // TODO: should we store these as database timestamps?
        // store ctime of this record
        assert(wg_set_field(db, rec, hobodb_base_field_ctime, enc_ctime) >= 0);
        assert(wg_set_field(db, rec, hobodb_base_field_atime, enc_atime) >= 0);
        assert(wg_set_field(db, rec, hobodb_base_field_uri, enc_uri) >= 0);
        assert(0 != wg_end_write(db, lock_id)); // unlock failure is catastrophic
        code = 0;
        break;
    }
    
    return code;

}


//
// This encodes the base fields; if it is the initial pass,
// we will set the type and uuid.  Once set, we treat those as
// invariant.
//
static int generic_relationship_encode(void *db, hobodb_relationship_t *relationship, int initial)
{
    wg_int enc_type;
    wg_int enc_uuid;
    wg_int enc_object1 = 0;
    wg_int enc_object2 = 0;
    wg_int enc_relationship = 0;
    wg_int enc_properties = 0;
    wg_int enc_attributes = 0;
    wg_int enc_labels = 0;
    wg_int lock_id = 0;
    int encoded = 0;
    char uuid_type[16];

    assert(NULL != db);
    assert(NULL != relationship);
    assert(NULL != relationship->record);

    assert(wg_get_record_len(db, relationship->record) >= hobodb_relationship_field_max);

    if (initial) {
        //
        // This is where we set up the immutable fields - once written, we don't change
        // them
        //
        assert(0 == wg_get_field(db, relationship->record, hobodb_base_field_type));
        assert(0 == wg_get_field(db, relationship->record, hobodb_base_field_uuid));
        assert(0 == wg_get_field(db, relationship->record, hobodb_relationship_field_object1));
        assert(0 == wg_get_field(db, relationship->record, hobodb_relationship_field_object2));
        assert(0 == wg_get_field(db, relationship->record, hobodb_relationship_field_relationship));

        record_type_to_uuid("relationship", relationship->type);
        assert(!uuid_is_null(relationship->type));

        uuid_type[sizeof(uuid_type)-1] = '\0';
        strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);
        enc_type = wg_encode_blob(db, (char *)relationship->type, uuid_type, (wg_int)sizeof(uuid_t));
        assert(WG_ILLEGAL != enc_type);

        enc_uuid = encode_uuid_as_string(db, relationship->uuid);
        assert(WG_ILLEGAL != enc_uuid);

        enc_object1 = encode_uuid_as_string(db, relationship->object1);
        assert(WG_ILLEGAL != enc_object1);

        enc_object2 = encode_uuid_as_string(db, relationship->object2);
        assert(WG_ILLEGAL != enc_object2);

        enc_relationship = wg_encode_blob(db, (char *)relationship->relationship, uuid_type, (wg_int)sizeof(uuid_t));
        assert(WG_ILLEGAL != enc_relationship);

    }
    else {
        // MUST not be NULL - and must be correct type(s)
        assert(WG_BLOBTYPE == wg_get_field(db, relationship->record, hobodb_base_field_type));
        assert(WG_STRTYPE == wg_get_field(db, relationship->record, hobodb_base_field_uuid));
        assert(WG_STRTYPE == wg_get_field(db, relationship->record, hobodb_relationship_field_object1));
        assert(WG_STRTYPE == wg_get_field(db, relationship->record, hobodb_relationship_field_object2));
        assert(WG_BLOBTYPE == wg_get_field(db, relationship->record, hobodb_relationship_field_relationship));
    }

    // Now encode the base part
    assert(0 == generic_base_encode(db, (hobodb_base_t *)relationship, initial));

    // and the relationship part
    enc_properties = wg_encode_blob(db, (char *)relationship->properties, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_properties);

    enc_attributes = wg_encode_blob(db, (char *)relationship->attributes, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_attributes);

    enc_labels = wg_encode_blob(db, (char *)relationship->labels, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_labels);

    lock_id = wg_start_write(db);
    while (0 != lock_id) {
        if (initial) {
            assert(wg_set_field(db, relationship->record, hobodb_relationship_field_object1, enc_object1) >= 0);
            assert(wg_set_field(db, relationship->record, hobodb_relationship_field_object2, enc_object2) >= 0);
            assert(wg_set_field(db, relationship->record, hobodb_relationship_field_relationship, enc_relationship) >= 0);
        }
        assert(wg_set_field(db, relationship->record, hobodb_relationship_field_properties, enc_properties) >= 0);
        assert(wg_set_field(db, relationship->record, hobodb_relationship_field_attributes, enc_attributes) >= 0);
        assert(wg_set_field(db, relationship->record, hobodb_relationship_field_labels, enc_labels) >= 0);
        assert(0 != wg_end_write(db, lock_id)); // unlock failure is catastrophic
        encoded = 1;
        break;
    }

    assert(encoded);
    return 0;
}

#if 0
static int hobodb_update_relationship(void *db, hobodb_relationship_t *relationship)
{
    int code = -EINVAL;
    wg_int enc_object1 = 0;
    wg_int enc_object2 = 0;
    wg_int enc_relationship = 0;
    wg_int enc_properties = 0;
    wg_int enc_attributes = 0;
    wg_int enc_labels = 0;
    wg_int lock_id = 0;
    void *rec = NULL;
    char uuid_type[16];


    memset(uuid_type, 0, sizeof(uuid_type));
    strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);

    assert(NULL != relationship);
    assert(NULL != relationship->record);
    rec = (void *) relationship->record;
    
    code = hobodb_update_base(db, (hobodb_base_t *) relationship);
    if (0 != code) {
        return code;
    }

    // store uuid of this record as a string, so we can search on it
    enc_object1 = encode_uuid_as_string(db, relationship->object1);
    assert(WG_ILLEGAL != enc_object1);

    enc_object2 = encode_uuid_as_string(db, relationship->object2);
    assert(WG_ILLEGAL != enc_object2);

    enc_relationship = wg_encode_blob(db, (char *)relationship->relationship, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_relationship);

    enc_properties = wg_encode_blob(db, (char *)relationship->properties, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_properties);

    enc_attributes = wg_encode_blob(db, (char *)relationship->attributes, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_attributes);

    enc_labels = wg_encode_blob(db, (char *)relationship->labels, uuid_type, (wg_int)sizeof(uuid_t));
    assert(WG_ILLEGAL != enc_labels);

    lock_id = wg_start_write(db);
    while (0 != lock_id) {
        // TODO: should we store these as database timestamps?
        // store ctime of this record

        assert(wg_set_field(db, rec, hobodb_relationship_field_object1, enc_object1) >= 0);
        assert(wg_set_field(db, rec, hobodb_relationship_field_object2, enc_object2) >= 0);
        assert(wg_set_field(db, rec, hobodb_relationship_field_relationship, enc_relationship) >= 0);
        assert(wg_set_field(db, rec, hobodb_relationship_field_properties, enc_properties) >= 0);
        assert(wg_set_field(db, rec, hobodb_relationship_field_attributes, enc_attributes) >= 0);
        assert(wg_set_field(db, rec, hobodb_relationship_field_attributes, enc_labels) >= 0);
        assert(0 != wg_end_write(db, lock_id)); // unlock failure is catastrophic
        code = 0;
        break;
    }

    return code;
}
#endif // 0

int hobodb_relationship_encode(void *db, hobodb_relationship_t *relationship)
{
    uuid_t relationship_type_uuid;
    char uuid_type[16];
    int initial = 0;

    record_type_to_uuid("relationship", relationship_type_uuid);
    assert(!uuid_is_null(relationship_type_uuid));
    assert(NULL != relationship);
    assert(0 == uuid_compare(relationship_type_uuid, relationship->type));
    memset(uuid_type, 0, sizeof(uuid_type)-1);
    strncpy(uuid_type, "uuid", sizeof(uuid_type)-1);

    if (NULL == relationship->record) {
        relationship->record = wg_create_record(db, hobodb_relationship_field_max);
        if (NULL == relationship->record) {
            return -ENOMEM;
        }
        initial = TRUE;

#if 0
        // store type field
        enc = wg_encode_blob(db, (char *)relationship->type, uuid_type, (wg_int)sizeof(uuid_t));
        assert(WG_ILLEGAL != enc);
        lock_id = wg_start_write(db);
        while (0 != lock_id) {
            assert(wg_set_field(db, rec, hobodb_base_field_uuid, enc) >= 0);
            assert(0 != wg_end_write(db, lock_id));

            hobodb_update_relationship(db, relationship);

            // Done
            break;
        }
#endif // 0
    }
    assert(0 == generic_relationship_encode(db, relationship, initial));

    return 0;
    
}


//
// generic_base_decode: works on decoding the base portion of the object
// _without_ insisting it has to be an object of a specific type (leave
// that check to the caller!)
//
static int generic_relationship_decode(void *db, hobodb_relationship_t *relationship)
{
    wg_int enc_object1 = 0;
    wg_int enc_object2 = 0;
    wg_int enc_relationship = 0;
    wg_int enc_properties = 0;
    wg_int enc_attributes = 0;
    wg_int enc_labels = 0;
    wg_int lock_id = 0;
    int decoded = 0;

    assert(NULL != db);
    assert(NULL != relationship);
    assert(NULL != relationship->record);

    assert(wg_get_record_len(db, relationship->record) >= hobodb_relationship_field_max);

    assert(0 == generic_base_decode(db, (hobodb_base_t *)relationship));

    lock_id = wg_start_read(db);
    while (0 != lock_id) {
        enc_object1 = wg_get_field(db, relationship->record, hobodb_relationship_field_object1);
        enc_object2 = wg_get_field(db, relationship->record, hobodb_relationship_field_object2);
        enc_relationship = wg_get_field(db, relationship->record, hobodb_relationship_field_relationship);
        enc_properties = wg_get_field(db, relationship->record, hobodb_relationship_field_properties);
        enc_attributes = wg_get_field(db, relationship->record, hobodb_relationship_field_attributes);
        enc_labels = wg_get_field(db, relationship->record, hobodb_relationship_field_labels);
        assert(0 != wg_end_read(db, lock_id)); // unlock failure = bad things
        decoded = 1;
        break;
    }
    assert(decoded);

    assert(WG_STRTYPE == wg_get_encoded_type(db, enc_object1));
    decode_uuid_from_string(db, enc_object1, relationship->object1);
    assert(!uuid_is_null(relationship->object1));

    assert(WG_STRTYPE == wg_get_encoded_type(db, enc_object2));
    decode_uuid_from_string(db, enc_object2, relationship->object2);
    assert(!uuid_is_null(relationship->object2));

    assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc_relationship));
    assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc_relationship));
    memcpy(&relationship->relationship, wg_decode_blob(db, enc_relationship), sizeof(uuid_t));

    assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc_properties));
    assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc_properties));
    memcpy(&relationship->properties, wg_decode_blob(db, enc_properties), sizeof(uuid_t));

    assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc_attributes));
    assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc_attributes));
    memcpy(&relationship->attributes, wg_decode_blob(db, enc_attributes), sizeof(uuid_t));

    assert(WG_BLOBTYPE == wg_get_encoded_type(db, enc_labels));
    assert(sizeof(uuid_t) == wg_decode_blob_len(db, enc_labels));
    memcpy(&relationship->labels, wg_decode_blob(db, enc_labels), sizeof(uuid_t));

    return 0;
}

int hobodb_relationship_decode(void *db, hobodb_relationship_t *relationship)
{
    uuid_t relationship_type_uuid;

    assert(NULL != db);
    assert(NULL != relationship);
    assert(NULL != relationship->record); // we can't decode without a record

    record_type_to_uuid("relationship", relationship_type_uuid);
    assert(!uuid_is_null(relationship_type_uuid));

    assert(0 == generic_relationship_decode(db, relationship));

    return 0;
}


static void init_relationship(hobodb_relationship_t *newrel)
{
    assert(NULL != newrel);
    memset(newrel, 0, sizeof(hobodb_relationship_t));

    init_base((hobodb_base_t *)newrel);
    record_type_to_uuid("relationship", newrel->type); // override
    uuid_clear(newrel->object1);
    uuid_clear(newrel->object2);
    uuid_clear(newrel->relationship);
    uuid_clear(newrel->properties);
    uuid_clear(newrel->attributes);
    uuid_clear(newrel->labels);

}

hobodb_relationship_t *hobodb_alloc_relationship(void)
{
    hobodb_relationship_t *newrel = (hobodb_relationship_t *)malloc(sizeof(hobodb_relationship_t));

    while (NULL != newrel) {
        memset(newrel, 0, sizeof(hobodb_relationship_t));
        init_relationship(newrel);
        break;
    }

    return newrel;
}



void hobodb_free_relationship(hobodb_relationship_t *relationship)
{
    if (NULL != relationship) {
        hobodb_cleanup_base((hobodb_base_t *)relationship);
        free(relationship);
        relationship = NULL;
    }
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

void *hobodb_lookup_object(void *db, uuid_t uuid)
{
    void *rec = NULL;
    char uuid_string[40];
    assert(NULL != db);
    assert(!uuid_is_null(uuid));

    uuid_unparse_lower(uuid, uuid_string);
    rec = wg_find_record_str(db, hobodb_base_field_uuid, WG_COND_EQUAL, uuid_string, NULL);
    return rec;
}

struct relationship_list {
    list_entry_t list_entry;
    hobodb_relationship_t *relationship;
};


void *hobodb_lookup_relationship(void *db, uuid_t object)
{
    void *rec = NULL;
    char uuid_string[40];
    wg_int lock_id;
    unsigned count;
    list_entry_t *list = NULL;
    struct relationship_list *list_entry = NULL;

    assert(NULL != db);
    assert(!uuid_is_null(object));
    
    list = malloc(sizeof(list_entry_t));
    initialize_list(list);
    uuid_unparse_lower(object, uuid_string);

    lock_id = wg_start_read(db);
    while (0 != lock_id) {

        // first look in the first field of the relationship
        for (count = 0, rec = wg_find_record_str(db, hobodb_relationship_field_object1, WG_COND_EQUAL, uuid_string, NULL);
             rec;
             rec = wg_find_record_str(db, hobodb_relationship_field_object1, WG_COND_EQUAL, uuid_string, rec)) {
                list_entry = malloc(sizeof(struct relationship_list));
                assert(NULL != list_entry);
                list_entry->relationship = hobodb_alloc_relationship();
                assert(NULL != list_entry->relationship);
                list_entry->relationship->record = rec;
                assert(0 == hobodb_relationship_decode(db, list_entry->relationship));
                insert_list_tail(list, &list_entry->list_entry);
                count = count + 1;
        }

        // now look in the second field of the relationship
        for (rec = wg_find_record_str(db, hobodb_relationship_field_object2, WG_COND_EQUAL, uuid_string, NULL);
             rec;
             rec = wg_find_record_str(db, hobodb_relationship_field_object2, WG_COND_EQUAL, uuid_string, rec)) {
                list_entry = malloc(sizeof(struct relationship_list));
                assert(NULL != list_entry);
                list_entry->relationship = hobodb_alloc_relationship();
                assert(NULL != list_entry->relationship);
                list_entry->relationship->record = rec;
                assert(0 == hobodb_relationship_decode(db, list_entry->relationship));
                insert_list_tail(list, &list_entry->list_entry);
                count = count + 1;
        }

        assert(0 != wg_end_read(db, lock_id)); // unlock failure is catastrophic
        break;
    }

    if (count > 0) {
        return list;
    }
    else {
        return NULL;
    }
}

hobodb_relationship_t *hobodb_lookup_relationship_next(void *list)
{
    struct relationship_list *list_entry = NULL;
    hobodb_relationship_t *relationship = NULL;

    if (empty_list(list)) {
        return NULL;
    }

    list_entry = container_of(remove_list_head(list), struct relationship_list, list_entry);
    relationship = list_entry->relationship;
    free(list_entry);
    return relationship;
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

typedef struct _hobo_relationship_type {
    list_entry_t list_entry;
    uuid_t relationship_uuid;
    const char *relationship_name;
} hobo_relationship_type_t;

static list_entry_t relationships_list = {&relationships_list, &relationships_list};
static pthread_rwlock_t relationship_type_lock = PTHREAD_RWLOCK_INITIALIZER;

static void lock_table_for_lookup(void)
{
    pthread_rwlock_rdlock(&relationship_type_lock);
}

static void lock_table_for_change(void)
{
    pthread_rwlock_wrlock(&relationship_type_lock);
}

static void unlock_table(void)
{
    pthread_rwlock_unlock(&relationship_type_lock);
}


// This routine is used to search the list - caller should hold the lock
static void lookup_relationship_by_name_unlocked(const char *relationship_name, uuid_t *relationship_uuid)
{
    hobo_relationship_type_t *rel;
    list_entry_t *le;

    assert(NULL != relationship_name);
    assert(strlen(relationship_name) > 0);
    assert(NULL != relationship_uuid);


    list_for_each(&relationships_list, le) {
        rel = container_of(le, hobo_relationship_type_t, list_entry);
        assert(NULL != rel->relationship_name);
        assert(!uuid_is_null(rel->relationship_uuid));
        if (0 == strcasecmp(relationship_name, rel->relationship_name)) {
            uuid_copy(*relationship_uuid, rel->relationship_uuid);
            return;
        }
    }

    // not found
    uuid_clear(*relationship_uuid);
}

// this routine is used to search the list - caller should hold the lock
static void lookup_relationship_by_uuid_unlocked(uuid_t relationship_uuid, char **relationship_name)
{
    hobo_relationship_type_t *rel;
    list_entry_t *le;

    assert(!uuid_is_null(relationship_uuid));
    assert(NULL != relationship_name);
    assert(NULL == *relationship_name); // caller shouldn't send a valid buffer

    list_for_each(&relationships_list, le) {
        rel = container_of(le, hobo_relationship_type_t, list_entry);
        assert(NULL != rel->relationship_name);
        assert(!uuid_is_null(rel->relationship_uuid));
        if (0 == uuid_compare(relationship_uuid, rel->relationship_uuid)) {
            *relationship_name = strdup(rel->relationship_name);
            return;
        }
    }

    *relationship_name = NULL; // not found
    return;
}


static void register_relationship_unlocked(hobo_relationship_type_t *new_relationship)
{
    assert(NULL != new_relationship);
    assert(NULL != new_relationship->relationship_name);
    assert(strlen(new_relationship->relationship_name) > 0);
    insert_list_tail(&relationships_list, &new_relationship->list_entry);
}

void hobo_lookup_relationship_by_name(const char *relationship_name, uuid_t *relationship_uuid)
{
    lock_table_for_lookup();
    lookup_relationship_by_name_unlocked(relationship_name, relationship_uuid);
    unlock_table();
}

void hobo_lookup_relationship_by_uuid(uuid_t relationship_uuid, char **relationship_name)
{
    lock_table_for_change();
    lookup_relationship_by_uuid_unlocked(relationship_uuid, relationship_name);
    unlock_table();
}

void hobo_register_relationship(uuid_t relationship_uuid, const char *relationship_name)
{
    uuid_t test_uuid;
    char *test_name = NULL;
    hobo_relationship_type_t *reltype = NULL;

    assert(!uuid_is_null(relationship_uuid));
    assert(NULL != relationship_name);
    assert(strlen(relationship_name) > 0);

    reltype = malloc(sizeof(hobo_relationship_type_t));
    assert(NULL != reltype);
    uuid_copy(reltype->relationship_uuid, relationship_uuid);
    reltype->relationship_name = strdup(relationship_name);

    lock_table_for_lookup();
    lookup_relationship_by_name_unlocked(relationship_name, &test_uuid);
    assert(uuid_is_null(test_uuid));
    lookup_relationship_by_uuid_unlocked(relationship_uuid, &test_name);
    assert(NULL == test_name);
    unlock_table();

    lock_table_for_change();
    register_relationship_unlocked(reltype);
    unlock_table();
}

