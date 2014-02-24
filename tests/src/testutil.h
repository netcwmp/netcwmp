/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TEST_UTIL
#define TEST_UTIL


#include <cwmp/types.h>
#include <cwmp/pool.h>
#include <cwmp/cfg.h>
#include <cwmp/session.h>
#include <cwmp/log.h>
#include <cwmp/event.h>
#include <cwmp/queue.h>
#include <cwmp/buffer.h>
#include <cwmp/cwmp.h>
#include <cwmp/model.h>
#include "abts.h"



/* XXX FIXME */
#ifdef WIN32
#define EXTENSION ".exe"
#elif NETWARE
#define EXTENSION ".nlm"
#else
#define EXTENSION
#endif


#define  STRING_MAX    8096

/* Some simple functions to make the test apps easier to write and
 * a bit more consistent...
 */



/* Assert that RV is an APR_SUCCESS value; else fail giving strerror
 * for RV and CONTEXT message. */
void assert_success(abts_case* tc, const char *context, 
                        int rv, int lineno);
#define  ASSERT_SUCCESS(tc, ctxt, rv) \
             assert_success(tc, ctxt, rv, __LINE__)

void initialize(void);


abts_suite *testsuite_model(abts_suite *suite);
abts_suite *testsuite_rpc(abts_suite *suite);
abts_suite *testsuite_pool(abts_suite *suite);

#endif /* APR_TEST_INCLUDES */


