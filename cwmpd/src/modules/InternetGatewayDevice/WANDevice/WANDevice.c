
int  cpe_refresh_igd_wandevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    child_param = param_node->child;
    if(child_param)
    {
        for(tmp_param=child_param->next_sibling; tmp_param; )
        {
            cwmp_log_info("refresh WANDevice node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;
                
        parameter_node_t * wan1_param;
        cwmp_model_copy_parameter(param_node, &wan1_param, 1);
        
        parameter_node_t * wan2_param;
        cwmp_model_copy_parameter(param_node, &wan2_param, 2);
       
        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
    
    return FAULT_CODE_OK;
}
    
    
