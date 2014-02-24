#include "testutil.h"

static void print_param(parameter_node_t * param, int level)
{
  if(!param) return; 
 
  parameter_node_t * child;
  char fmt[64];
  //cwmp_log_debug("name: %s, type: %s, level: %d\n", param->name, cwmp_get_type_string(param->type), level);
  int i=0;
  
  sprintf(fmt, "|%%-%d\s%%s", level*4);
  
  cwmp_log_debug(fmt, "----", param->name);
  child = param->child;

  if(!child)
	return;
  print_param(child, level+1);

  parameter_node_t * next = child->next_sibling;

  while(next)
 {
    print_param(next, level+1);
    next = next->next_sibling;
 }

	
}

static void test_cwmp_model_load (abts_case *tc, void *data)
{
    cwmp_t * c = (cwmp_t*)data;
    FUNCTION_TRACE();
  
    
}



static void test_cwmp_model_copy_parameter (abts_case *tc, void *data)
{
    cwmp_t * c = (cwmp_t*)data;
    char * param_name = "InternetGatewayDevice.WANDevice.";
    FUNCTION_TRACE();
    parameter_node_t * param = cwmp_get_parameter_path_node(c->root, param_name);
    
     
    parameter_node_t * new_param;
    cwmp_model_copy_parameter(param, &new_param, 1);
    print_param(c->root,0);
    
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.");
    cwmp_model_copy_parameter(param, &new_param, 1);
    print_param(c->root,0);
    
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.");
    cwmp_model_copy_parameter(param, &new_param, 2);
    print_param(c->root,0);
    
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.1.");
    cwmp_model_delete_parameter(param);
    print_param(c->root,0);
    
    
    param = cwmp_get_parameter_path_node(c->root, "InternetGatewayDevice.WANDevice.");
    cwmp_model_copy_parameter(param, &new_param, 2);
    print_param(c->root,0);
    
}




extern cwmp_t * gcwmp;
abts_suite *testsuite_model(abts_suite *suite)
{
    suite = ADD_SUITE(suite)
  
    abts_run_test(suite, test_cwmp_model_load, gcwmp);
    abts_run_test(suite, test_cwmp_model_copy_parameter, gcwmp);
     
    
    return suite;
}

 
