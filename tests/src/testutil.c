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

#include <stdio.h>
#include <stdlib.h>

#include "abts.h"
#include "testutil.h"


cwmp_t * gcwmp;
pool_t * gpool;

static char *native_strerror(int statcode, char *buf,
		size_t bufsize)
{
	const char *err = strerror(statcode);
	if (err) {
		return strncpy(buf, err, bufsize);
	} else {
		return strncpy(buf,  "APR does not understand this error code", bufsize);
	}
}

void  assert_success(abts_case* tc, const char* context, int rv, 
		int lineno) 
{
	if (rv == 500) {
		abts_not_impl(tc, context, lineno);
	} else if (rv != CWMP_OK) {
		char buf[STRING_MAX], ebuf[128];
		sprintf(buf, "%s (%d): %s\n", context, rv,
				native_strerror(rv, ebuf, sizeof ebuf));
		abts_fail(tc, buf, lineno);
	}
}

void initialize(void) {

	cwmp_log_init(NULL, CWMP_LOG_DEBUG);

	gpool = (pool_t*)pool_create(POOL_DEFAULT_SIZE);
	gcwmp = (cwmp_t*)pool_palloc(gpool, sizeof(cwmp_t));
	if(!gcwmp)
	{
		cwmp_log_error("create cwmp_t error!");
		exit(-1);
	}
	cwmp_log_debug("cwmp at %p", gcwmp);
	if(!gcwmp->pool)
	{
		cwmp_log_debug("cwmp pool at %p", gpool);
		gcwmp->pool = gpool;   
	}

	cwmp_model_load_xml(gcwmp, "device.xml", NULL, 0);

	char * envstr;
	char * encstr;

	envstr = "SOAP-ENV"; //cwmp_conf_get("cwmp:soap_env");
	encstr = "SOAP-ENC"; // cwmp_conf_get("cwmp:soap_enc");

	cwmp_set_envelope_ns(envstr, encstr);

}
