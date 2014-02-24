#include "testutil.h"




static void test_cwmp_get_parameter_path_node (abts_case *tc, void *data)
{
    cwmp_t * c = (cwmp_t*)data;

    parameter_node_t * param;
    FUNCTION_TRACE();
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.");
    ASSERT_NOTNULL(param);
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.{i}.WANConnectionNumberOfEntries");
    ASSERT_NOTNULL(param);
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.1.WANConnectionNumberOfEntries");
    ASSERT_NULL(param);
    
}



static void test_getset_parameter_node_value (abts_case *tc, void *data)
{
    cwmp_t * c = (cwmp_t*)data;

    parameter_node_t * param;
    char * name = "InternetGatewayDevice.DeviceInfo.SoftwareVersion";
    char * retval;
    char * value ;
    
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    FUNCTION_TRACE();
    param = cwmp_get_parameter_path_node(c->root, name);
    ASSERT_NOTNULL(param);
    
    value = "V1.3.x";

    cwmp_set_parameter_node_value(c, param, name, value, strlen(value));
    
    retval = cwmp_data_get_parameter_value(c, param, name, pool);
    printf("retval ------------------is %s\n", retval);
    ASSERT_STR_EQ(value, retval);
   
    
    value = "V1.4.x";
    cwmp_set_parameter_node_value(c, param, name, value, strlen(value));
    retval = cwmp_data_get_parameter_value(c, param, name, pool);
    ASSERT_STR_EQ(value, retval);
    
    
    pool_destroy(pool);

}

static void test_cwmp_session_create_getparameternames(abts_case *tc, void *data)
{
    cwmp_t * cwmp = (cwmp_t*)data;
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    xmldoc_t *  doc, *newdoc;
    char * message = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
                       xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns=\"urn:dslforum-org:cwmp-1-0\"><SOAP-ENV:Header><cwmp:ID>129479899093788892</cwmp:ID><cwmp:NoMoreRequests>0</cwmp:NoMoreRequests></SOAP-ENV:Header><SOAP-ENV:Body><cwmp:GetParameterNames><ParameterPath>InternetGatewayDevice.</ParameterPath><NextLevel>1</NextLevel></cwmp:GetParameterNames></SOAP-ENV:Body></SOAP-ENV:Envelope>"
    ;

    cwmp_session_t * session = cwmp_session_create(cwmp);
    cwmp_session_open(session);


    cwmp_uint32_t msglength = strlen(message);
    size_t len;

    char *xmlbuf = pool_palloc(pool, msglength+32);

    len = sprintf(xmlbuf,"<cwmp>");
    strncpy(xmlbuf + len, message, msglength);
    strcpy(xmlbuf + len + msglength, "</cwmp>");

    printf("agent analyse xml: \n%s", xmlbuf);

    doc = XmlParseBuffer(pool, xmlbuf);
    ASSERT_NOTNULL(doc);

    newdoc = cwmp_session_create_getparameternames_response_message(session, doc, pool);

    cwmp_write_doc_to_chunk(newdoc, session->writers,  pool);

    cwmp_chunk_print(session->writers);

    pool_destroy(pool);

}

static void test_cwmp_session_create_addobject(abts_case *tc, void *data)
{
    cwmp_t * cwmp = (cwmp_t*)data;
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    xmldoc_t *  doc, *newdoc;
    char * message = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
                       xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns=\"urn:dslforum-org:cwmp-1-0\"><SOAP-ENV:Header><cwmp:ID soap:mustUnderstand=\"1\">1234</cwmp:ID></SOAP-ENV:Header>  \
                       <SOAP-ENV:Body><cwmp:AddObject><ObjectName>InternetGatewayDevice.WANDevice.</ObjectName><ParameterKey></ParameterKey></cwmp:AddObject></SOAP-ENV:Body></SOAP-ENV:Envelope>"
    ;
   

   printf("create session\n"); 
    cwmp_session_t * session = cwmp_session_create(cwmp);
    pool_create(POOL_DEFAULT_SIZE);
   pool_create(POOL_DEFAULT_SIZE);
  pool_create(POOL_DEFAULT_SIZE);
 pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
pool_create(POOL_DEFAULT_SIZE);
 printf("open session \n"); 
    cwmp_session_open(session);
  
   

       
    cwmp_uint32_t msglength = strlen(message);
    size_t len;

    char *xmlbuf = pool_palloc(pool, msglength+32);

    len = sprintf(xmlbuf,"<cwmp>");
    strncpy(xmlbuf + len, message, msglength);
    strcpy(xmlbuf + len + msglength, "</cwmp>");
//    strcat(xmlbuf, "</cwmp>");
 //   xmlbuf[len+msglength+7] = '\0';

    printf("agent analyse xml: \n%s", xmlbuf);

    doc = XmlParseBuffer(pool, xmlbuf);
    ASSERT_NOTNULL(doc);
    
    newdoc = cwmp_session_create_addobject_response_message(session, doc, pool);
    
    cwmp_write_doc_to_chunk(newdoc, session->writers,  pool);
    
    cwmp_chunk_print(session->writers);
    
    pool_destroy(pool);

}




static void test_cwmp_session_create_deleteobject(abts_case *tc, void *data)
{
    cwmp_t * cwmp = (cwmp_t*)data;
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    xmldoc_t *  doc, *newdoc;
    char * message = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
                       xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns=\"urn:dslforum-org:cwmp-1-0\"><SOAP-ENV:Header><cwmp:ID soap:mustUnderstand=\"1\">1234</cwmp:ID></SOAP-ENV:Header>  \
                       <SOAP-ENV:Body><cwmp:DeleteObject><ObjectName>InternetGatewayDevice.WANDevice.1</ObjectName><ParameterKey></ParameterKey></cwmp:DeleteObject></SOAP-ENV:Body></SOAP-ENV:Envelope>"
    ;
    
    cwmp_session_t * session = cwmp_session_create(cwmp);
    cwmp_session_open(session);
   
       
    cwmp_uint32_t msglength = strlen(message);
    size_t len;

    char *xmlbuf = pool_palloc(pool, msglength+32);

    len = sprintf(xmlbuf,"<cwmp>");
    strncpy(xmlbuf + len, message, msglength);
    strcat(xmlbuf, "</cwmp>");

    printf("agent analyse xml: \n%s", xmlbuf);

    doc = XmlParseBuffer(pool, xmlbuf);
    ASSERT_NOTNULL(doc);
    
    newdoc = cwmp_session_create_deleteobject_response_message(session, doc, pool);
    
    cwmp_write_doc_to_chunk(newdoc, session->writers,  pool);
    
    cwmp_chunk_print(session->writers);
    
    pool_destroy(pool);

}



extern cwmp_t * gcwmp;
abts_suite *testsuite_rpc(abts_suite *suite)
{
    suite = ADD_SUITE(suite)
  
  
//    abts_run_test(suite, test_cwmp_get_parameter_path_node, gcwmp);
    
//    abts_run_test(suite, test_getset_parameter_node_value, gcwmp);
       
//   abts_run_test(suite, test_cwmp_session_create_getparameternames, gcwmp);

 
    abts_run_test(suite, test_cwmp_session_create_addobject, gcwmp);
        
//    abts_run_test(suite, test_cwmp_session_create_deleteobject, gcwmp);
    
    
    return suite;
}

 
 
 
 
