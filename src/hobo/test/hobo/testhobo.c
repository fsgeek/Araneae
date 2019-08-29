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
    munit_assert(0 == hobodb_base_encode(db, base, &record));
    munit_assert(NULL != record);
    munit_assert(base->record == record);

    hobodb_free_base(base);

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

