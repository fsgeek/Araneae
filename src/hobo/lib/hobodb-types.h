//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//
#if !defined(__HOBODB_TYPES_H__)
#define __HOBODB_TYPES_H__ (1)

typedef enum {
    hobo_base_object_type = 100,
    hobo_relationship_object_type = 200,
    hobo_property_object_type = 300,
    hobo_attribute_object_type = 400,
    hobo_label_object_type = 500,
} hobo_object_types_t;

#endif // !defined(__HOBODB_TYPES_H__)
