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

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <pthread.h>
#include "munit.h"
#include <errno.h>
#include <hobodb.h>
#include <hobodb-util.h>

#if !defined(__notused)
#define __notused __attribute__((unused))
#endif //

static const char *hobo_db_test_name = "hobo-test";
static int testdb_initialized = 0;

static void *open_hobodb(void)
{
    char dbname[16];
    void *db = NULL;

    // TODO: initialization

    while (!testdb_initialized) {
        // initialization logic goes here

        // TODO: this is where I'd want to load in any initial state from the persistent store
        memset(dbname, 0, sizeof(dbname));
        strncpy(dbname, hobo_db_test_name, sizeof(dbname) - sizeof(char)); // the annoyance of APIs that take non-const strings - can't pass it directly w/o a compiler warning.
        wg_delete_database(dbname); // always start with a clean database
        db = wg_attach_database(dbname, 200 * 1024 * 1024); // 200MB for now, not sure about the name
        assert(NULL != db);

        //
        // Done
        //
        testdb_initialized = 1;
    }

    return db;
}


static void close_hobodb(void *db)
{
    assert(NULL != db);

    wg_detach_database(db);

    testdb_initialized = 0;
}

static MunitResult
test_one(
    const MunitParameter params[] __notused,
    void *prv __notused
)
{
    return MUNIT_OK;
}

static MunitResult
test_dbopen(
    const MunitParameter params[] __notused,
    void *prv __notused
)
{
    void *db;

    db = open_hobodb();
    munit_assert(NULL != db);

    close_hobodb(db);

    return MUNIT_OK;
}

static MunitResult
test_db_base(
    const MunitParameter params[] __notused,
    void *prv __notused
)
{
    void *db;
    hobodb_base_t *base;
    void *record = NULL;
    uuid_t uuid;

    db = open_hobodb();
    munit_assert(NULL != db);

    base = hobodb_alloc_base();
    munit_assert(NULL != base);

    // need to set values
    base->ctime = time(NULL);
    base->uri.prefix = strdup("file");
    base->uri.name = strdup("/mnt/hobo");
    base->atime = time(NULL);

    // now encode
    munit_assert(0 == hobodb_base_encode(db, base));
    munit_assert(NULL != base->record);
    uuid_copy(uuid, base->uuid);
    hobodb_free_base(base);

    //
    // let's look up the record we just created
    //
    record = hobodb_lookup_object(db, uuid);
    munit_assert(NULL != record);

    base = hobodb_alloc_base();
    munit_assert(NULL != base);
    munit_assert(NULL == base->record);
    base->record = record;

    munit_assert(0 == hobodb_base_decode(db, base));

    munit_assert(0 == strcmp("file", base->uri.prefix));
    munit_assert(0 == strcmp("/mnt/hobo", base->uri.name));

    close_hobodb(db);

    return MUNIT_OK;
}



/** Print single record
 *
 */
static void print_record(void *db, wg_int* rec,  FILE *logf) 
{

  wg_int len, enc;
  int i;
  char strbuf[256];

  if (rec==NULL) {
    fprintf(logf, "<null rec pointer>\n");
    return;
  }

  len = wg_get_record_len(db, rec);
  fprintf(logf, "[");
  for(i=0; i<len; i++) {
    if(i) fprintf(logf, ",");
    enc = wg_get_field(db, rec, i);
    wg_snprint_value(db, enc, strbuf, 255);
    fprintf(logf, "%s", strbuf);
  }
  fprintf(logf, "]");
}


static void dump_db(void *db)
{
    void *rec = NULL;
    FILE *logf = NULL;

    logf = fopen("/tmp/testhobo.log", "w");
    fprintf(logf, "Start db dump\n");
    rec = wg_get_first_record(db);
    while(rec) {
        print_record(db, (wg_int *) rec, logf);
        fprintf(logf, "\n");
        rec = wg_get_next_record(db,rec);
    }
    fprintf(logf, "end db dump\n");
    fflush(logf);
}

static MunitResult
test_db_relationship(
    const MunitParameter params[] __notused,
    void *prv __notused
)
{
    void *db;
    void *record = NULL;
    hobodb_base_t *object1 = NULL;
    hobodb_base_t *object2 = NULL;
    hobodb_relationship_t *relationship = NULL;
    hobodb_relationship_t *relationship2 = NULL;
    uuid_t relationship_uuid;
    hobodb_relationship_t *relationship_found = NULL;
    unsigned count = 0;
    hobo_util_create_parameters_t hobo_params;

    db = open_hobodb();
    munit_assert(NULL != db);

    // create a pair of objects
    
    memset(&hobo_params, 0, sizeof(hobo_params));
    hobo_params.type = hobo_base_object_type;
    hobo_params.parameters.base_object_parameters.uri.prefix = "file";
    hobo_params.parameters.base_object_parameters.uri.name = "foo";
    object1 = hobo_util_create_object(&hobo_params);
    munit_assert(NULL != object1);
    munit_assert(0 == hobo_util_encode_object(db, object1));

    memset(&hobo_params, 0, sizeof(hobo_params));
    hobo_params.type = hobo_base_object_type;
    hobo_params.parameters.base_object_parameters.uri.prefix = "file";
    hobo_params.parameters.base_object_parameters.uri.name = "bar";
    object2 = hobo_util_create_object(&hobo_params);
    munit_assert(NULL != object2);
    munit_assert(0 == hobo_util_encode_object(db, object2));

    // now create a relationship between them
    // TODO: need to pre-define common relationship types
    //       Also, probably need a way to permanently define these, in order to permit "portability"
    //       obvious answer: create an object, using this as the uuid, then associate a property with it.
    uuid_generate(relationship_uuid); 
    memset(&hobo_params, 0, sizeof(hobo_params));
    hobo_params.type = hobo_relationship_object_type;
    uuid_copy(hobo_params.parameters.relationship_object_parameters.object1, object1->uuid);
    uuid_copy(hobo_params.parameters.relationship_object_parameters.object2, object2->uuid);
    uuid_copy(hobo_params.parameters.relationship_object_parameters.relationship, relationship_uuid);
    relationship = hobo_util_create_object(&hobo_params);
    munit_assert(NULL != relationship);

    //
    // Now we need to encode this
    //
    munit_assert(0 == hobo_util_encode_object(db, relationship));


    //
    // let's look up the record we just created
    //
    // (1) Find it
    // (2) allocate an object for it
    // (3) decode it
    //
    // Note: at some point this needs to be tied up into a single operation
    //
    record = hobodb_lookup_object(db, relationship->uuid);
    munit_assert(NULL != record);

    relationship2 = hobodb_alloc_relationship();
    munit_assert(NULL != relationship2);
    relationship2->record = record;

    munit_assert(0 == hobodb_relationship_decode(db, relationship));

    dump_db(db);

    //
    // Now let's try to find all the relationships for a given object
    //
    void *results = hobodb_lookup_relationship(db, object1->uuid);
    assert(NULL != results);

    while ((relationship_found = hobodb_lookup_relationship_next(results))) {
        assert(0 == uuid_compare(relationship->uuid, relationship_found->uuid));
        hobodb_free_relationship(relationship_found);
        count++;
    }
    assert(count > 0);


    //
    // Declare victory and clean up
    //
    close_hobodb(db);
    return MUNIT_OK;
}



#define TEST(_name, _func, _params)             \
    {                                           \
        .name = (_name),                        \
        .test = (_func),                        \
        .setup = NULL,                          \
        .tear_down = NULL,                      \
        .options = MUNIT_TEST_OPTION_NONE,      \
        .parameters = (_params),                     \
    }

int
main(
    int argc,
    char **argv)
{
    static MunitTest tests[] = {
        TEST((char *)(uintptr_t)"/one", test_one, NULL),
        TEST((char *)(uintptr_t)"/hobodb/open", test_dbopen, NULL),
        TEST((char *)(uintptr_t)"/hobodb/db_base", test_db_base, NULL),
        TEST((char *)(uintptr_t)"/hobodb/db_relationship", test_db_relationship, NULL),
        TEST(NULL, NULL, NULL),
    };
    static const MunitSuite suite = {
        .prefix = (char *)(uintptr_t)"/hobo",
        .tests = tests,
        .suites = NULL,
        .iterations = 1,
        .options = MUNIT_SUITE_OPTION_NONE,
    };

    return munit_suite_main(&suite, NULL, argc, argv);
}

