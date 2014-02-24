#include "cwmp/model.h"
#include "data_model.h"
#include "cwmp_module.h"
#include "InternetGatewayDevice/InternetGatewayDevice.c"

model_func_t ModelFunction[] =
{
    {"cpe_get_igd_di_manufacturer", cpe_get_igd_di_manufacturer},
    {"cpe_get_igd_di_manufactureroui", cpe_get_igd_di_manufactureroui},
    {"cpe_get_igd_di_productclass", cpe_get_igd_di_productclass},
    {"cpe_get_igd_di_serialnumber", cpe_get_igd_di_serialnumber},
    {"cpe_get_igd_di_specversion", cpe_get_igd_di_specversion},
    {"cpe_get_igd_di_hardwareversion", cpe_get_igd_di_hardwareversion},
    {"cpe_get_igd_di_softwareversion", cpe_get_igd_di_softwareversion},
    {"cpe_get_igd_di_provisioningcode", cpe_get_igd_di_provisioningcode},

    {"cpe_get_igd_ms_username", cpe_get_igd_ms_username},
    {"cpe_get_igd_ms_password", cpe_get_igd_ms_password},
    {"cpe_get_igd_ms_connectionrequesturl", cpe_get_igd_ms_connectionrequesturl},
    {"cpe_get_igd_ms_connectionrequestusername", cpe_get_igd_ms_connectionrequestusername},
    {"cpe_get_igd_ms_connectionrequestpassword", cpe_get_igd_ms_connectionrequestpassword},
    {"cpe_set_igd_ms_connectionrequestusername", cpe_set_igd_ms_connectionrequestusername},
    {"cpe_set_igd_ms_connectionrequestpassword", cpe_set_igd_ms_connectionrequestpassword},


	{"cpe_refresh_igd_wandevice", cpe_refresh_igd_wandevice},
    {"cpe_refresh_igd_wanconnectiondevice", cpe_refresh_igd_wanconnectiondevice},
    {"cpe_refresh_igd_wanipconnection", cpe_refresh_igd_wanipconnection},

};

int get_index_after_paramname(parameter_node_t * param, const char * tag_name)
{
    parameter_node_t * parent;
    parameter_node_t * tmp;
    for(parent=param->parent, tmp = param; parent; tmp = parent, parent = parent->parent)
    {
        if(TRstrcmp(parent->name, tag_name) == 0)
        {
             if(is_digit(tmp->name) == 0)
             {
                return TRatoi(tmp->name);   
             }
        }        
    }
    return -1;
}


void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile)
{  

    cwmp_model_load_xml(cwmp, xmlfile, ModelFunction, sizeof(ModelFunction)/sizeof(model_func_t));
}


