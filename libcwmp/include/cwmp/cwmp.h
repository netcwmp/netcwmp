/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMP_H__
#define __CWMP_H__

#include <cwmp/xmlet.h>
#include <cwmp/types.h>
#include <cwmp/util.h>
#include <cwmp/memory.h>
#include <cwmp/buffer.h>
#include <cwmp/event.h>
#include <cwmp/queue.h>


#define ASSERT assert
#define XMLCAST(x)  ((XmlChar *)(x))






#define CWMP_HEAD_MAX   64
#define CWMP_NAME_MAX   32
#define CWMP_PARAMVALUE_SIZE	512

#define SOAP_ENV_NS  "http://schemas.xmlsoap.org/soap/envelope/"
#define SOAP_ENC_NS "http://schemas.xmlsoap.org/soap/encoding/"
#define SOAP_XSI_NS  "http://www.w3.org/1999/XMLSchema-instance"
#define SOAP_XSD_NS  "http://www.w3.org/1999/XMLSchema"
#define SOAP_CWMP_NS "urn:dslforum-org:cwmp-1-0"



#define SOAP_ENV_DEFAULT		"SOAP-ENV"
#define SOAP_ENC_DEFAULT		"SOAP-ENC"
#define SOAP_XML_HEADER			"Header"
#define SOAP_XML_BODY	  		"Body"
#define SOAP_XML_ENVELOPE  		"Envelope"
#define SOAP_XML_FAULT			"Fault"



#define CWMP_XML_HEADER_ID	         		"cwmp:ID"
#define CWMP_XML_HEADER_HOLDREQUESTS	 	    "cwmp:HoldRequests"
#define CWMP_XML_HEADER_NOMOREREQUESTS	 	    "cwmp:NoMoreRequests"
#define CWMP_XML_INFORM_MAXENVELOPES	    "MaxEnvelopes"
#define CWMP_XML_GETPARAMETERNAMES_PARAMETERPATH	 	    "ParameterPath"
#define CWMP_XML_GETPARAMETERNAMES_NEXTLEVEL	 			"NextLevel"
#define CWMP_XML_COMMANDKEY	"CommandKey"



#define SOAP_XSI_TYPE            "xsi:type"
#define SOAP_XSI_NAME            "xsi:name"

#define SOAP_XSD_STRING          "xsd:string"
#define SOAP_XSD_UNSIGNEDINT     "xsd:unsignedInt"
#define SOAP_XSD_ANY             "xsd:any"
#define SOAP_XSD_DATETIME        "xsd:dateTime"
#define SOAP_TYPE_ARRAYTYPE        "arrayType"



#define CWMP_XML_MUSTUNDERSTAND "mustUnderstand"

#define CWMP_RPC_INFORM		 						"cwmp:Inform"
#define CWMP_RPC_INFORMRESPONSE		 				"cwmp:InformResponse"
#define CWMP_RPC_GETPARAMETERNAMES  				"cwmp:GetParameterNames"
#define CWMP_RPC_GETPARAMETERNAMESRESPONSE          "cwmp:GetParameterNamesResponse"
#define CWMP_RPC_GETPARAMETERVALUES  				"cwmp:GetParameterValues"
#define CWMP_RPC_GETPARAMETERVALUESRESPONSE         "cwmp:GetParameterValuesResponse"
#define CWMP_RPC_SETPARAMETERVALUES  				"cwmp:SetParameterValues"
#define CWMP_RPC_SETPARAMETERVALUESRESPONSE         "cwmp:SetParameterValuesResponse"
#define CWMP_RPC_GETRPCMETHODS  					"cwmp:GetRPCMethods"
#define CWMP_RPC_GETRPCMETHODSRESPONSE  		    "cwmp:GetRPCMethodsResponse"
#define CWMP_RPC_DOWNLOAD	 						"cwmp:Download"
#define CWMP_RPC_DOWNLOADRESPONSE	 				"cwmp:DownloadResponse"
#define CWMP_RPC_UPLOAD	 							"cwmp:Upload"
#define CWMP_RPC_UPLOADRESPONSE	 					"cwmp:UploadResponse"
#define CWMP_RPC_REBOOT	 							"cwmp:Reboot"
#define CWMP_RPC_REBOOTRESPONSE	 					"cwmp:RebootResponse"
#define CWMP_RPC_ADDOBJECT	 						"cwmp:AddObject"
#define CWMP_RPC_ADDOBJECTRESPONSE	 				"cwmp:AddObjectResponse"
#define CWMP_RPC_DELETEOBJECT	 					"cwmp:DeleteObject"
#define CWMP_RPC_DELETEOBJECTRESPONSE	 			"cwmp:DeleteObjectResponse"
#define CWMP_RPC_TRANSFERCOMPLETE	 			"cwmp:TransferComplete"
#define CWMP_RPC_TRANSFERCOMPLETERESPONSE 			"cwmp:TransferCompleteResponse"
#define CWMP_RPC_FACTORYRESET               "cwmp:FactoryReset"
#define CWMP_RPC_FACTORYRESETRESPONSE       "cwmp:FactoryResetResponse"



#define CWMP_INFORM_EVENT_CODE_0  "0 BOOTSTRAP"
#define CWMP_INFORM_EVENT_CODE_1  "1 BOOT"
#define CWMP_INFORM_EVENT_CODE_2  "2 PERIODIC"
#define CWMP_INFORM_EVENT_CODE_3  "3 SCHEDULED"
#define CWMP_INFORM_EVENT_CODE_4  "4 VALUE CHANGE"
#define CWMP_INFORM_EVENT_CODE_5  "5 KICKED"
#define CWMP_INFORM_EVENT_CODE_6  "6 CONNECTION REQUEST"
#define CWMP_INFORM_EVENT_CODE_7  "7 TRANSFER COMPLETE"
#define CWMP_INFORM_EVENT_CODE_8  "8 DIAGNOSTICS COMPLETE"
#define CWMP_INFORM_EVENT_CODE_9  "9 REQUEST DOWNLOAD"
#define CWMP_INFORM_EVENT_CODE_10  "10 AUTONOMOUS TRANSFER COMPLETE"
#define CWMP_INFORM_EVENT_CODE_11  "M Reboot"
#define CWMP_INFORM_EVENT_CODE_12  "M ScheduleInform"
#define CWMP_INFORM_EVENT_CODE_13  "M Download"
#define CWMP_INFORM_EVENT_CODE_14  "M Upload"
#define CWMP_INFORM_EVENT_CODE_15  "M X_0_AccountChanged"
#define CWMP_INFORM_EVENT_CODE_16  "M 16"
#define CWMP_INFORM_EVENT_CODE_17  "M 17"

#define FAULT_CODE_OK			0
#define FAULT_CODE_9000              9000
#define FAULT_CODE_9001              9001
#define FAULT_CODE_9002              9002
#define FAULT_CODE_9003              9003
#define FAULT_CODE_9004              9004
#define FAULT_CODE_9005              9005
#define FAULT_CODE_9006              9006
#define FAULT_CODE_9007              9007
#define FAULT_CODE_9008              9008
#define FAULT_CODE_9009              9009
#define FAULT_CODE_9010              9010
#define FAULT_CODE_9011              9011
#define FAULT_CODE_9012              9012
#define FAULT_CODE_9013              9013
#define FAULT_CODE_9014              9014
#define FAULT_CODE_9015              9015
#define FAULT_CODE_9016              9016
#define FAULT_CODE_9017              9017
#define FAULT_CODE_9018              9018
#define FAULT_CODE_9019              9019



#define FAULT_STR_9000  "Method not supported"
#define FAULT_STR_9001  "Request denied"
#define FAULT_STR_9002  "Internal error"
#define FAULT_STR_9003  "Invalid arguments"
#define FAULT_STR_9004  "Resources exceeded"
#define FAULT_STR_9005  "Invalid parameter name"
#define FAULT_STR_9006  "Invalid parameter type"
#define FAULT_STR_9007  "Invalid parameter value"
#define FAULT_STR_9008  "Attempt to set a non-writable parameter"
#define FAULT_STR_9009  "Notification request rejected"
#define FAULT_STR_9010  "Download failure"
#define FAULT_STR_9011  "Upload failure"
#define FAULT_STR_9012  "File transfer server authentication failure"
#define FAULT_STR_9013  "Unsupported protocol for file transfer"
#define FAULT_STR_9014  "Download failure: unable to join multicast group"
#define FAULT_STR_9015  "Download failure: unable to contact file server"
#define FAULT_STR_9016  "Download failure: unable to access file"
#define FAULT_STR_9017  "Download failure: unable to complete download"
#define FAULT_STR_9018  "Download failure: file corrupted"
#define FAULT_STR_9019  "Download failure: file authentication failure"

#define FAULT_STRING(x)	cwmp_get_fault_string(x)


#define InternetGatewayDeviceModule         "InternetGatewayDevice"
#define DeviceSummaryModule                 "DeviceSummary"
#define DeviceInfoModule                    "DeviceInfo"

#define ManufacturerModule                  "Manufacturer"
#define ManufacturerOUIModule               "ManufacturerOUI"
#define ProductClassModule                  "ProductClass"
#define DescriptionModule                   "Description"
#define SerialNumberModule                  "SerialNumber"
#define SpecVersionModule                   "SpecVersion"
#define HardwareVersionModule               "HardwareVersion"
#define SoftwareVersionModule               "SoftwareVersion"
#define ProvisioningCodeModule              "ProvisioningCode"
#define UpTimeModule                        "UpTime"
#define DeviceLogModule                     "DeviceLog"


#define ManagementServerModule              "ManagementServer"
#define URLModule                           "URL"
#define UsernameModule                      "Username"
#define PasswordModule                      "Password"
#define PeriodicInformIntervalModule        "PeriodicInformInterval"
#define PeriodicInformTimeModule            "PeriodicInformTime"
#define PeriodicInformEnableModule          "PeriodicInformEnable"
#define ParameterKeyModule                  "ParameterKey"
#define ConnectionRequestURLModule          "ConnectionRequestURL"
#define ConnectionRequestUsernameModule     "ConnectionRequestUsername"
#define ConnectionRequestPasswordModule     "ConnectionRequestPassword"
#define UpgradesManagedModule               "UpgradesManaged"
#define UDPConnectionRequestAddressModule   "UDPConnectionRequestAddress"
#define STUNEnableModule                    "STUNEnable"
#define STUNServerAddressModule             "STUNServerAddress"
#define STUNServerPortModule                "STUNServerPort"
#define STUNUsernameModule                  "STUNUsernameb"
#define STUNPasswordModule                  "STUNPassword"
#define NATDetectedModule                   "NATDetected"


#define DeviceConfigModule                  "DeviceConfig"
#define PersistentDataModule                "PersistentData"
#define ConfigFileModule                    "ConfigFile"


#define TimeModule                          "Time"
#define NTPServer1Module                    "NTPServer1"
#define NTPServer2Module                    "NTPServer2"
#define CurrentLocalTimeModule              "CurrentLocalTime"
#define LocalTimeZoneModule                 "LocalTimeZone"
#define LocalTimeZoneNameModule             "LocalTimeZoneName"
#define DaylightSavingsUsedModule           "DaylightSavingsUsed"
#define DaylightSavingsStartModule          "DaylightSavingsStart"
#define DaylightSavingsEndModule            "DaylightSavingsEnd"


#define DeviceSummaryModule                 "DeviceSummary"

#define WANDeviceModule                     "WANDevice"

#define WANConnectionDeviceModule           "WANConnectionDevice"
#define WANIPConnectionModule               "WANIPConnection"
#define ExternalIPAddressModule             "ExternalIPAddress"








typedef enum
{
	CWMP_EMPTY_METHOD,
	CWMP_INFORM_METHOD,
	CWMP_INFORMRESPONSE_METHOD,
	CWMP_GETPARAMETERNAMES_METHOD,
	CWMP_GETPARAMETERNAMESRESPONSE_METHOD,
	CWMP_GETPARAMETERVALUES_METHOD,
	CWMP_GETPARAMETERVALUESRESPONSE_METHOD,
	CWMP_SETPARAMETERVALUES_METHOD,
	CWMP_SETPARAMETERVALUESRESPONSE_METHOD,
	CWMP_GETRPCMETHODS_METHOD,
	CWMP_GETRPCMETHODSRESPONSE_METHOD,
	CWMP_DOWNLOAD_METHOD,
	CWMP_DOWNLOADRESPONSE_METHOD,
	CWMP_UPLOAD_METHOD,
	CWMP_UPLOADRESPONSE_METHOD,
	CWMP_REBOOT_METHOD,
	CWMP_REBOOTRESPONSE_METHOD,
	CWMP_ADDOBJECT_METHOD,
	CWMP_ADDOBJECTRESPONSE_METHOD,
	CWMP_DELETEOBJECT_METHOD,
	CWMP_DELETEOBJECTRESPONSE_METHOD


}cwmp_method_t;




enum cwmp_type_t
{
	TYPE_OBJECT=0,	//obj
	TYPE_INT,	//int
	TYPE_UNSIGNEDINT, //uint
	TYPE_STRING,  	//s
	TYPE_STRING16,	//s16
	TYPE_STRING32,	//s32
	TYPE_STRING64,	//s64
	TYPE_STRING128,	//s128
	TYPE_STRING256,	//s256
	TYPE_STRING1024, //s1024
	TYPE_DATETIME,	//dt
	TYPE_BOOLEAN,	//bool
	TYPE_BASE64,	//base
	TYPE_ANY,
	TYPE_UNKNOWN
};

enum InformEventType
{
	INFORM_BOOTSTRAP = 0,
	INFORM_BOOT,
	INFORM_PERIODIC,
	INFORM_SCHEDULED,
	INFORM_VALUECHANGE,
	INFORM_KICKED,
	INFORM_CONNECTIONREQUEST,
	INFORM_TRANSFERCOMPLETE,
	INFORM_DIAGNOSTICSCOMPLETE,
	INFORM_REQUESTDOWNLOAD,
	INFORM_AUTONOMOUSTRANSFERCOMPLETE,
	INFORM_MREBOOT,
	INFORM_MSCHEDULEINFORM,
	INFORM_MDOWNLOAD,
	INFORM_MUPLOAD,
	INFORM_ACCOUNTCHANGE,
	INFORM_MVENDORSPECRPC,
	INFORM_XOUIEVENT,
	INFORM_MAX
};

struct cwmp_st
{
	cwmp_t			* old_cwmp;
	int new_request;
	int httpd_port;

	int    cpe_auth;
	int    acs_auth;

	char * acs_url;
	char * cpe_mf;
	char * cpe_oui;
	char * cpe_sn;
	char * cpe_pc;
	char * cpe_name;

	char * acs_user;
	char * acs_pwd;
	char * cpe_user;
	char * cpe_pwd;

	char * event_filename;

	pthread_mutex_t     event_mutex;
	
	event_list_t * el;

	int	event_count;

	event_global_t    event_global;

	transfer_t    transfer_info;      //ÓÃÓÚDownloadºÍUpload

	queue_t  *	queue;

	pool_t * pool;
	parameter_node_t * root;

	

#ifdef USE_CWMP_OPENSSL
    SSL_CTX * ssl_ctx;
#endif

};




struct parameter_node_attr_st
{
	cwmp_byte_t   type;     /* 0:single 1:multi */
	cwmp_byte_t   nc;             /* Notification,0:off,1:passive,2:active */
	cwmp_uint64_t   acl;            /* access list */
	cwmp_uint64_t	ext;
};

#define PARAMETER_NODE_V_PADDING	0,0,0,0,0

struct parameter_node_st
{
	char *    name;
	cwmp_byte_t	rw;	//read / writable
	cwmp_byte_t     type;
	
	size_t		value_length;
	char *          value;
	parameter_node_attr_t attr;

	parameter_node_t * parent;
	parameter_node_t * child;
	parameter_node_t * prev_sibling;
	parameter_node_t * next_sibling;

	parameter_get_handler_pt    	get;
	parameter_set_handler_pt    	set;
	parameter_notify_handler_pt 	notify;
	parameter_add_handler_pt  	add;
	parameter_del_handler_pt	del;
	parameter_refresh_handler_pt	refresh;



	cwmp_uint32_t	max;
	cwmp_uint32_t	ext1;
	cwmp_uint32_t	ext2;
	cwmp_uint32_t	ext3;
	cwmp_uint32_t	ext4;

};


struct datatime_st
{
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int min;
    unsigned int sec;
};



struct download_arg_st
{
    char            *cmdkey;
    char            *filetype;
    char            *url;
    char            *username;
    char            *password;
    char            *targetname;
    char            *succurl;
    char            *failurl;
    unsigned int   delaysec;
    unsigned int   filesize;
};


struct upload_arg_st
{
    char            *cmdkey;
    char            *filetype;
    char            *url;
    char            *username;
    char            *password;
    unsigned int   delaysec;
};


struct fault_code_st
{
	int fault_code;
	char * fault_string;
};



typedef struct  envelope_t envelope_t;


void cwmp_set_envelope_ns(const char * envstr, const char * encstr);


char * cwmp_xml_get_node_attribute(xmlnode_t * node, const char * name);


parameter_list_t* cwmp_create_parameter_list(env_t * env );

parameter_t* cwmp_create_parameter(env_t * env ,  const char * name, const char * value, size_t value_length, int type);

int cwmp_create_parameter_node(env_t * env ,  parameter_node_t ** news, const char * name);

download_arg_t * cwmp_clone_download_arg(download_arg_t * dlarg);
upload_arg_t * cwmp_clone_upload_arg(upload_arg_t * ularg);

xmldoc_t* cwmp_create_inform_message(env_t * env ,  header_t * header,
		device_id_t * deviceid,
		event_list_t *events,
		datatime_t * currentt,
		unsigned int max_envelope,
		unsigned int retry_count,
		parameter_list_t * pl);

xmldoc_t* cwmp_create_getparameternames_response_message(env_t * env ,
		header_t * header,
		const char * path_name,
		parameter_node_t * node,
		unsigned int next_subset,
		unsigned int next_level);

xmldoc_t *  cwmp_create_getparametervalues_response_message(env_t * env ,
    header_t * header,
    parameter_list_t * pl);

xmldoc_t *  cwmp_create_setparametervalues_response_message(env_t * env ,
    header_t * header,
    unsigned int status);

xmldoc_t *  cwmp_create_getrpcmethods_response_message(env_t * env ,
    header_t * header,
    char ** methods,
    unsigned int count);

xmldoc_t *  cwmp_create_reboot_response_message(env_t * env ,
    header_t * header);

xmldoc_t * cwmp_create_download_response_message(env_t * env , header_t * header, int status);
xmldoc_t * cwmp_create_upload_response_message(env_t * env , header_t * header, int status);
xmldoc_t * cwmp_create_transfercomplete_message(env_t * env ,  header_t * header, event_code_t * evcode);
xmldoc_t * cwmp_create_addobject_response_message(env_t * env , header_t * header, int instances, int status);
xmldoc_t * cwmp_create_deleteobject_response_message(env_t * env , header_t * header, int status);
xmldoc_t * cwmp_create_faultcode_response_message(env_t * env , header_t * header, fault_code_t * fault);
xmldoc_t * cwmp_create_faultcode_setparametervalues_response_message(env_t * env , header_t * header, parameter_list_t * param_list, fault_code_t * fault);
xmldoc_t * cwmp_create_factoryreset_response_message(env_t * env ,  header_t * header);

event_code_t * cwmp_create_event_code(env_t * env );
event_list_t * cwmp_create_event_list(env_t * env, int  size );

int     cwmp_add_parameter_to_list(env_t * env ,  parameter_list_t * pl, parameter_t * parameter);
void    cwmp_add_event_to_list(env_t * env ,  event_list_t * eventlist, event_code_t * event);

int     cwmp_parse_getparameternames_message(env_t * env, xmldoc_t * doc, char ** path_name, unsigned int * next_level, fault_code_t *fault);
int     cwmp_parse_getparametervalues_message(env_t * env ,   xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault);
int     cwmp_parse_setparametervalues_message(env_t * env ,   xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault);
int     cwmp_parse_reboot_message(env_t * env , xmldoc_t *doc, char ** key, fault_code_t *fault);
int cwmp_parse_addobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root,  int * instances, int* status, fault_code_t *fault);
int cwmp_parse_deleteobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root, int* status, fault_code_t *fault);

xmlnode_t * cwmp_get_rpc_method_node(xmldoc_t *  doc);
xmlnode_t * cwmp_get_header_node(xmldoc_t *  doc);

int     cwmp_parse_header_node(xmlnode_t * node, header_t ** header, pool_t * pool);
char *  cwmp_get_rpc_method_name(xmldoc_t *  doc);
char *  cwmp_get_type_string(int type);
int	cwmp_get_type_value(char * type);
char *  cwmp_get_fault_string(int code);

parameter_node_t * cwmp_get_parameter_node(parameter_node_t * root, const char * param_name);
parameter_node_t * cwmp_get_parameter_path_node(parameter_node_t * parent, const char * param_name);

int cwmp_get_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, char ** value, pool_t * pool);
int cwmp_set_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, const char * value, int value_length);
int     cwmp_write_doc_to_chunk(xmldoc_t *  doc, cwmp_chunk_t * chunk, pool_t * pool);

xmldoc_t * cwmp_xml_parse_buffer(pool_t * pool, char * buffer);
xmlnode_t * cwmp_xml_get_child_with_name(void * nodeptr, const char * nodeName);


#define XmlNodeGetDocRoot(docptr)    XmlNodeGetFirstChild(XmlNodeGetFirstChild(& (docptr)->node))




#endif // CWMP_H

