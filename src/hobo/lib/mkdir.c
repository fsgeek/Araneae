//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include "hobo.h"

static void hmkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
{
    hobo_object_t *pob = NULL;
    hobo_object_t *nob = NULL;
    hobodb_base_t *dbpob = NULL;
    hobodb_base_t *dbnob = NULL;
    hobodb_relationship_t *dbrob = NULL;
    struct fuse_entry_param e;
    struct stat *nob_statbuf = NULL;
    hobo_util_create_parameters_t params;
    uuid_t relationship_uuid;

    fprintf(stderr, "HoboFS: called %s, req = 0x%p, parent = %lu, name = %s, mode = 0x%x\n", __PRETTY_FUNCTION__, req, parent, name, mode);

    memset(&e, 0, sizeof(e));

    hobodb_lookup_record_type_uuid("relationship", relationship_uuid);
    assert(0 == uuid_is_null(relationship_uuid));

    pob = hobo_object_lookup_by_ino(parent);
    if (NULL == pob) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    // lookup existing object
    dbpob = hobodb_lookup_object(hobo_db, pob->uuid);
    assert(NULL != dbpob);

    // create a new object - note that there's NO "crash protection" here.
    memset(&params, 0, sizeof(params));
    params.type = hobo_base_object_type;
    params.parameters.base_object_parameters.uri.prefix = NULL;
    params.parameters.base_object_parameters.uri.name = "";
    dbnob = hobo_util_create_object(&params);
    assert(NULL != dbnob);


    // Add a relationship
    memset(&params, 0, sizeof(params));
    params.type = hobo_relationship_object_type;
    uuid_copy(params.parameters.relationship_object_parameters.object1, dbpob->uuid);
    uuid_copy(params.parameters.relationship_object_parameters.object2, dbnob->uuid);
    params.parameters.base_object_parameters.uri.prefix = NULL;
    params.parameters.base_object_parameters.uri.name = strdup(name);
    uuid_copy(params.parameters.relationship_object_parameters.relationship, relationship_uuid);
    dbrob = (hobodb_relationship_t *)hobo_util_create_object(&params); // parent<->child
    assert(NULL != dbrob);

    // TODO: add this to the inode cache

    nob_statbuf = malloc(sizeof(struct stat));
    assert(NULL != nob_statbuf);

    nob_statbuf->st_ino = 0; // TODO: I need a number here...
    nob_statbuf->st_mode = S_IFDIR | mode;
    nob_statbuf->st_nlink = 1;
    nob_statbuf->st_uid = getuid();
    nob_statbuf->st_gid = getgid();
    nob_statbuf->st_size = 0x1000;
    nob_statbuf->st_blocks = 1;
    nob_statbuf->st_atime = dbnob->atime;
    nob_statbuf->st_mtime =
    nob_statbuf->st_ctime = dbnob->ctime;

    // Now commit the objects
    assert(0 == hobo_util_encode_object(hobo_db, dbnob));
    assert(0 == hobo_util_encode_object(hobo_db, (hobodb_base_t *)dbrob));

    // add to the inode table:
    //    (1) get an inode number
    //    (2) insert, using the uuid assigned by the database
    //    (3) make sure the insertion worked
    //
    nob_statbuf->st_ino = hobo_get_inode_number();

    // quick sanity check - make sure this isn't a duplicate
    assert(NULL == hobo_object_lookup_by_ino(nob_statbuf->st_ino));
    assert(NULL == hobo_object_lookup_by_uuid(&dbnob->uuid));

    nob = hobo_object_create(nob_statbuf->st_ino, &dbnob->uuid, &nob_statbuf);
    assert(NULL != nob);

    // respond
    fuse_reply_entry(req, &e);

    return;
}


hobo_mkdir_t hobo_mkdir = hmkdir;







