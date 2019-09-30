//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#include "hobodb.h"
#include "hobodb-types.h"


typedef struct {
    const uuid_t uuid;
    const char *uuid_string;
    const char *name;
    hobo_object_types_t type;
} hobo_record_type_t;

hobo_record_type_t hobo_record_type_uuids[] = {
    {{0x40, 0x7d, 0x21, 0x37, 0x7b, 0x37, 0x42, 0xeb, 0xaa, 0x95, 0x04, 0x9c, 0x35, 0xa6, 0x1d, 0xd1}, "407d2137-7b37-42eb-aa95-049c35a61dd1", "base", hobo_base_object_type},
    {{0x22, 0x8d, 0xee, 0x9c, 0x79, 0x92, 0x4c, 0x1a, 0xbc, 0xbd, 0xe7, 0xc9, 0x9e, 0x86, 0x40, 0xf3}, "228dee9c-7992-4c1a-bcbd-e7c99e8640f3", "relationship", hobo_relationship_object_type},
    {{0x33, 0x79, 0xe3, 0x77, 0x5a, 0x1a, 0x48, 0x10, 0x88, 0xde, 0xb6, 0xe6, 0x5d, 0x97, 0xee, 0x19}, "3379e377-5a1a-4810-88de-b6e65d97ee19", "property", hobo_property_object_type},
    {{0x2d, 0xa1, 0xeb, 0xd3, 0x60, 0x98, 0x4c, 0xfd, 0x84, 0x80, 0xed, 0xf4, 0x3b, 0xab, 0xfa, 0xfd}, "2da1ebd3-6098-4cfd-8480-edf43babfafd", "attribute", hobo_attribute_object_type},
    {{0x06, 0xd1, 0x19, 0xe4, 0x58, 0x93, 0x40, 0xea, 0x95, 0x65, 0x31, 0x3b, 0x77, 0x2b, 0x85, 0x73}, "06d119e4-5893-40ea-9565-313b772b8573", "label", hobo_label_object_type},
    {},
};

// This checks the veracity of the data.
static void check_record_types(void)
{
    uuid_t uuid;

    for (unsigned index = 0; 
         (index < (sizeof(hobo_record_type_uuids)/sizeof(hobo_record_type_t))) && (hobo_record_type_uuids[index].name != NULL); 
        index++) {
            uuid_parse(hobo_record_type_uuids[index].uuid_string, uuid);
            assert(0 == uuid_compare(uuid, hobo_record_type_uuids[index].uuid));
    }
}

const char *hobodb_lookup_record_type_uuid_string(const char *name)
{
    const char *found = NULL;

    for (unsigned index = 0; 
         (index < (sizeof(hobo_record_type_uuids)/sizeof(hobo_record_type_t))) && (hobo_record_type_uuids[index].name != NULL); 
        index++) {
        if (0 == strcmp(hobo_record_type_uuids[index].name, name)) {
            found = hobo_record_type_uuids[index].uuid_string;
            break;
        }
    }
    return found;
}

const char *hobodb_lookup_record_type_name(const uuid_t uuid)
{
    char uuid_string[40];
    const char *found = NULL;

    uuid_unparse_lower(uuid, uuid_string);

    for (unsigned index = 0; 
         (index < (sizeof(hobo_record_type_uuids)/sizeof(hobo_record_type_t))) && (hobo_record_type_uuids[index].name != NULL); 
        index++) {
        if (0 == strcmp(hobo_record_type_uuids[index].uuid_string, uuid_string)) {
            found = hobo_record_type_uuids[index].name;
            break;
        }
    }
    return found;
}

void hobodb_lookup_record_type_uuid(const char *name, uuid_t uuid)
{
    uuid_clear(uuid);
    static int checked = 0;

    if (!checked) {
        check_record_types();
        checked = 1;
    }

    for (unsigned index = 0; 
         (index < (sizeof(hobo_record_type_uuids)/sizeof(hobo_record_type_t))) && (hobo_record_type_uuids[index].name != NULL); 
        index++) {
        if (0 == strcmp(hobo_record_type_uuids[index].name, name)) {
            uuid_copy(uuid, hobo_record_type_uuids[index].uuid);
            break;
        }
    }

}
