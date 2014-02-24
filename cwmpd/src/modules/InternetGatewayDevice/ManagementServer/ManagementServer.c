

int cpe_get_localip(const char * eth_name, char *hostip)
{
    register int fd,intrface,retn=0;
    struct ifreq buf[32];
    struct ifconf ifc;
    char domain_host[100] = {0};
    char local_ip_addr[20] = {0};
    char local_mac[20] = {0};
    //Get Domain Name --------------------------------------------------
    strcpy(local_ip_addr, "127.0.0.1");
    if (!hostip)
        return -1;
    if (getdomainname(&domain_host[0], 100) != 0)
    {
        return -1;
    }
    //------------------------------------------------------------------
    //Get IP Address & Mac Address ----------------------------------------
    if ((fd=socket(AF_INET,SOCK_DGRAM,0))>=0)
    {
        ifc.ifc_len=sizeof buf;
        ifc.ifc_buf=(caddr_t)buf;
        if (!ioctl(fd,SIOCGIFCONF,(char*)&ifc))
        {
            intrface=ifc.ifc_len/sizeof(struct ifreq);
            while (intrface-->0)
            {
                if (!(ioctl(fd,SIOCGIFFLAGS,(char*)&buf[intrface])))
                {
                    if (buf[intrface].ifr_flags&IFF_PROMISC)
                    {
                        retn++;
                    }
                }
                //Get IP Address
                if (!(ioctl(fd,SIOCGIFADDR,(char*)&buf[intrface])))
                {
		    if(strcmp(eth_name, buf[intrface].ifr_name) == 0)
		    {
                    sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
		
		    break;
		    }
                }
                //Get Hardware Address

            }//While
        }
    }
    if ( fd > 0 )
    {
        close(fd);
    }

    strcpy(hostip, local_ip_addr);

    return CWMP_OK;
}

//InternetGatewayDevice.ManagementServer.Username
int cpe_get_igd_ms_username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	  *value = cwmp_conf_pool_get(pool, "cwmp:acs_username");
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.Username
int cpe_set_igd_ms_username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //save password to database or config file
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.Password
int cpe_get_igd_ms_password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	  *value = cwmp_conf_pool_get(pool, "cwmp:acs_password");
	  return FAULT_CODE_OK;
}

int cpe_set_igd_ms_password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //save password to database or config file
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.URL
int cpe_get_igd_ms_url(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	  *value = cwmp_conf_pool_get(pool, "cwmp:acs_url");
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.URL
int cpe_set_igd_ms_url(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //save password to database or config file
    return FAULT_CODE_OK;
}




//InternetGatewayDevice.ManagementServer.ConnectionRequestURL
int cpe_get_igd_ms_connectionrequesturl(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char buf[256]={0};
    char local_ip[32]={0};
    cpe_get_localip("eth1", local_ip);
    int port = cwmp_conf_get_int("cwmpd:httpd_port");
    snprintf(buf, 256, "http://%s:%d", local_ip, port);
    *value = PSTRDUP(buf);
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.ConnectionRequestUsername
int cpe_get_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_username");
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{

    return FAULT_CODE_OK;
}



//InternetGatewayDevice.ManagementServer.ConnectionRequestPassword
int cpe_get_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_password");
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_conf_set("cwmp:cpe_password", value);
    return FAULT_CODE_OK;
}



