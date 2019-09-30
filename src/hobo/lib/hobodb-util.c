//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#include "hobodb-util.h"
#include "hobodb.h"

//
// create_base_object
//
// prefix - the URI prefix (optional)
// name - the URI name portion (required)
//
static hobodb_base_t *create_base_object(const char *prefix, const char *name)
{
    hobodb_base_t *object = NULL;

    assert(NULL != name); // required

    object = hobodb_alloc_base();
    while (NULL != object) {

        object->ctime = time(NULL);
        object->atime = object->ctime;
        if (NULL != prefix) {
            object->uri.prefix = strdup(prefix);
        } else {
            object->uri.prefix = NULL;
        }
        object->uri.name = strdup(name);
        break;
    }

    return object;
}

//
// create_relationship_object
//
// object1 - first object (required)
// object2 - second object (required)
// relationship_uuid - identifies the type of relationship (required)
//
static hobodb_relationship_t *create_relationship_object(const uuid_t object1, const uuid_t object2, const uuid_t relationship_uuid)
{
    hobodb_relationship_t *object = NULL;

    assert(!uuid_is_null(object1)); // not optional
    assert(!uuid_is_null(object2)); // not optional
    assert(!uuid_is_null(relationship_uuid));

    object = hobodb_alloc_relationship();

    while (NULL != object) {
        uuid_copy(object->object1, object1);
        uuid_copy(object->object2, object2);
        uuid_copy(object->relationship, relationship_uuid);
        uuid_clear(object->properties);
        uuid_clear(object->attributes);
        uuid_clear(object->labels);
        break;
    }
    return object;
}

static hobodb_properties_t *create_property_object(const uuid_t object, const uuid_t property_uuid)
{
    assert(0 != uuid_is_null(object));
    assert(0 != uuid_is_null(property_uuid));
    assert(0); // not implemented yet
    return NULL;
}

static hobodb_attributes_t *create_attribute_object(const uuid_t object, const uuid_t attribute_uuid)
{
    assert(0 != uuid_is_null(object));
    assert(0 != uuid_is_null(attribute_uuid));
    assert(0); // not implemented yet
    return NULL;
}

static hobodb_label_t *create_label_object(const uuid_t object, const uuid_t label_uuid, const char *value)
{
    assert(0 != uuid_is_null(object));
    assert(0 != uuid_is_null(label_uuid));
    assert(NULL != value);
    assert(0); // not implemented yet
    return NULL;
}


//
// hobo_util_create_object
//   Used to create a new (in memory) object.
//
// parameters - the parameters to use when creating the object
//
hobodb_base_t *hobo_util_create_object(hobo_util_create_parameters_t *param) 
{
    void *object = NULL;

    switch(param->type) {
        default: {
            // unknown type
            assert(0);
            break;
        }
        case hobo_base_object_type: {
            object = create_base_object(param->parameters.base_object_parameters.uri.prefix, 
                                        param->parameters.base_object_parameters.uri.name);
            break;
        }
        case hobo_relationship_object_type: {
            object = create_relationship_object(param->parameters.relationship_object_parameters.object1, 
                                                param->parameters.relationship_object_parameters.object2, 
                                                param->parameters.relationship_object_parameters.relationship);
            break;
        }
        case hobo_property_object_type: {
            object = create_property_object(param->parameters.property_object_parameters.object, 
                                            param->parameters.property_object_parameters.property);
            break;
        }
        case hobo_attribute_object_type: {
            object = create_attribute_object(param->parameters.attribute_object_parameters.object, 
                                             param->parameters.attribute_object_parameters.property);
            break;
        }
        case hobo_label_object_type: {
            object = create_label_object(param->parameters.label_object_parameters.object,
                                         param->parameters.label_object_parameters.uuid,
                                         param->parameters.label_object_parameters.label);
            break;
        }
    }

    return (hobodb_base_t *)object;
}

static uuid_t base_type_uuid;
static uuid_t relationship_type_uuid;
static uuid_t property_type_uuid;
static uuid_t attribute_type_uuid;
static uuid_t label_type_uuid;
static int type_uuid_init = 0;

static void init_type_uuids(void) {

    if (0 == type_uuid_init) {
        hobodb_lookup_record_type_uuid("base", base_type_uuid);
        assert(0 == uuid_is_null(base_type_uuid));
        hobodb_lookup_record_type_uuid("relationship", relationship_type_uuid);
        assert(0 == uuid_is_null(relationship_type_uuid));
        hobodb_lookup_record_type_uuid("property", property_type_uuid);
        assert(0 == uuid_is_null(property_type_uuid));
        hobodb_lookup_record_type_uuid("attribute", attribute_type_uuid);
        assert(0 == uuid_is_null(attribute_type_uuid));
        hobodb_lookup_record_type_uuid("label", label_type_uuid);
        assert(0 == uuid_is_null(label_type_uuid));
        type_uuid_init = 1;
    }
}




//
// hobo_util_encode_object
//   Used to encode the given object against the specified database
//
// db - the database to use for encoding
// object - the object to encode
//
// returns 0 for success, anything else is an error
//
int hobo_util_encode_object(void *db, hobodb_base_t *object)
{
    int result = -1;

    assert(NULL != object); // required

    if (0 == type_uuid_init) {
        init_type_uuids();
    }

    if (0 == uuid_compare(base_type_uuid, object->type)) {
            result = hobodb_base_encode(db, object);
    }
    else if (0 == uuid_compare(relationship_type_uuid, object->type)) {
            result = hobodb_relationship_encode(db, (hobodb_relationship_t *)object);
    }
    else if (0 == uuid_compare(property_type_uuid, object->type)) {
            result = hobodb_properties_encode(db, (hobodb_properties_t *)object);
    }
    else if (0 == uuid_compare(attribute_type_uuid, object->type)) {
            result = hobodb_attributes_encode(db, (hobodb_attributes_t *)object);
    }
    else if (0 == uuid_compare(label_type_uuid, object->type)) {
            result = hobodb_labels_encode(db, (hobodb_label_t *)object);
    }
    else {
        // unknown type
        assert(0);
    }
    return result;
}

//
// hobo_util_update_object
//   Used to update an existing object against the specified database
//
// db - the database being updated
// object - the object to update
//
// returns 0 for success, anything else is an error
//
int hobo_util_update_object(void *db, hobodb_base_t *object)
{
    int result = -1;

    assert(NULL != object); // required

    if (0 == type_uuid_init) {
        init_type_uuids();
    }

    if (0 == uuid_compare(base_type_uuid, object->type)) {
        result = hobodb_update_base(db, object);
    }
    else if (0 == uuid_compare(relationship_type_uuid, object->type)) {
        result = hobodb_update_relationship(db, (hobodb_relationship_t *)object);
    }
    else if (0 == uuid_compare(property_type_uuid, object->type)) {
        result = hobodb_update_properties(db, (hobodb_properties_t *)object);
    }
    else if (0 == uuid_compare(attribute_type_uuid, object->type)) {
        result = hobodb_update_attributes(db, (hobodb_attributes_t *)object);
    }
    else if (0 == uuid_compare(label_type_uuid, object->type)) {
        result = hobodb_update_label(db, (hobodb_label_t *)object);
    }
    else {
        // unknown type
        assert(0);
    }

    return result;
}

hobodb_base_t *hobo_util_lookup_object(void *db, const uuid_t uuid)
{
    return (hobodb_base_t *)hobodb_lookup_object(db, uuid);
}