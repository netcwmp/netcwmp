#ifndef __CWMP_MODEL_H__
#define __CWMP_MODEL_H__

#include <cwmp/types.h>
#include <cwmp/cwmp.h>



#define DEVICE_MODEL_NODE   "model"
#define DEVICE_MODEL_OBJECT_NODE   "object"
#define DEVICE_MODEL_PARAM_NODE   "param"

struct model_func_st
{
   const char * name;
   void * func;
};

int cwmp_model_load_xml(cwmp_t * cwmp, const char * xmlfile, model_func_t * func_list, int func_count);
int cwmp_model_copy_parameter(parameter_node_t * param, parameter_node_t ** new_param, int instance_number);
int cwmp_model_refresh_object(cwmp_t * cwmp, parameter_node_t *param, int flag, callback_register_func_t callback_reg);



#endif
