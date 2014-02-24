netcwmp for TR069

How to:


build netcwmp:
1) ./configure
2) make
3) make install
4) cp conf/cwmp.conf /etc        /*   copy conf/cwmp.conf into /etc   */
5) run# cwmpd


see log file cwmpd.log in /var/log/cwmpd.log


Have fun!





Supported Functions:
    * SOAP/XML Parser 
    * SSL 
    * HTTP Server 
    * HTTP Client 
    * Ini config file Parser 
    * Digest Authentication 
    * GetRPCMethods 
    * Inform 
    * SetParameterValues 
    * GetParameterValues 
    * GetParameterNames
    * Download 
    * Upload 
    * AddObject
    * DeleteObject
    * FactoryReset
    * TransferComplete
    * Reboot 
    * TR-069 Object Models Interface 





Config File:

acs_auth£º          ACS auth CPE
cpe_auth:           CPE auth ACS
acs_url:            ACS URL
cpe_manufacture:    Manufacture
cpe_oui:            OUI
cpe_sn:             CPE Serial Number
cpe_name:           CPE Name
cpe_pc:             CPE Product Class
cpe_username:       InternetGatewayDevice.ManagementServer.ConnectionRequestUsername
cpe_password:       InternetGatewayDevice.ManagementServer.ConnectionRequestPassword
acs_username:       InternetGatewayDevice.ManagementServer.Username
acs_password:       InternetGatewayDevice.ManagementServer.Password
httpd_port:         Http server listen port
ca_file:            ca pem file
ca_password:        ca password

License:  Apache License
