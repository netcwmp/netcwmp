#ifndef __CWMP_DATA_MODEL_H__
#define __CWMP_DATA_MODEL_H__

#include <cwmp/cwmp.h>

int get_index_after_paramname(parameter_node_t * param, const char * tag_name);
void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile);

#endif
