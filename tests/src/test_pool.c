#include <cwmp/pool.h>
#include "testutil.h"


static void test_pool(abts_case *tc, void*data)
{
    pool_t * p = pool_create(POOL_DEFAULT_SIZE);

    char * c = pool_palloc(p, 1024);

    pool_clear(p);


pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);

    c = pool_palloc(p,3048);
    c = pool_palloc(p,3048);

    c = pool_palloc(p,5200);

    pool_clear(p);

    c = pool_palloc(p, 2048);
    c = pool_palloc(p,6000);

    c = pool_palloc(p, 2048);
        c = pool_palloc(p, 2048);
            c = pool_palloc(p, 2048);
    c = pool_palloc(p, 2048);

    c = pool_palloc(p, 2048);

        c = pool_palloc(p, 2048);




    pool_destroy(p);
}



abts_suite *testsuite_pool(abts_suite *suite)
{
        suite = ADD_SUITE(suite)


        abts_run_test(suite, test_pool, NULL);

    return suite; 
}


