 
#include "cwmp/http.h"
#include "cwmp/log.h"
#include "cwmp_private.h"
#include <cwmp/md5.h>





#ifdef USE_CWMP_OPENSSL

static char openssl_password[32];


BIO *bio_err=0;
static char *pass;
static int password_cb(char *buf,int num,
  int rwflag,void *userdata);
static void sigpipe_handle(int x);

/* A simple error and exit routine*/
int err_exit(string)
  char *string;
  {
    cwmp_log_error("%s\n",string);
    //exit(0);
  }

/* Print SSL errors and exit*/
int berr_exit(string)
  char *string;
  {
    cwmp_log_error("%s\n",string);
    ERR_print_errors(bio_err);
    //exit(0);
  }

/*The password code is not thread safe*/
static int password_cb(char *buf,int num,
  int rwflag,void *userdata)
  {
    if(num<strlen(pass)+1)
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
  }

static void sigpipe_handle(int x){
}

SSL_CTX *openssl_initialize_ctx(char *keyfile,char *password)
{
    SSL_METHOD *meth;
    SSL_CTX *ctx;
    
    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();
      
      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    //signal(SIGPIPE,sigpipe_handle);
    
    /* Create our context*/
    meth=SSLv23_method();
    ctx=SSL_CTX_new(meth);

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_chain_file(ctx,
      keyfile)))
      berr_exit("Can't read certificate file");

    pass=password;
    SSL_CTX_set_default_passwd_cb(ctx,
      password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx,
      keyfile,SSL_FILETYPE_PEM)))
      berr_exit("Can't read key file");

    /* Load the CAs we trust*/
    /*
    if(!(SSL_CTX_load_verify_locations(ctx,
      CA_LIST,0)))
      berr_exit("Can't read CA list");
    */
    
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,1);
#endif
    
    return ctx;
  }
     
void openssl_destroy_ctx(ctx)
  SSL_CTX *ctx;
  {
    SSL_CTX_free(ctx);
  }


SSL * openssl_connect(SSL_CTX * ctx, int fd)
{
    BIO *sbio;
    SSL * ssl=SSL_new(ctx);
            sbio=BIO_new_socket(fd,BIO_NOCLOSE);
            SSL_set_bio(ssl,sbio,sbio);
        
            if(SSL_connect(ssl)<=0)
            {
              cwmp_log_alert("SSL connect error");
              SSL_free(ssl);
              return NULL;
            }
            else
                {
                cwmp_log_info("SSL connect to host ok.\n");  
                     
            }
            
      return ssl;
    
}



/*

static int openssl_password_cb(char *buf, int num, int rwflag, void *userdata)
{    
    if (num < strlen(openssl_password)+1)
	    return 0;
    
    strcpy(buf,openssl_password);
    return(strlen(buf));
}


void openssl_init(void) {
    
}



int openssl_verify_callback(int ok, X509_STORE_CTX *store) {
    char data[256];
 
    if (!ok) {
        X509 *cert = X509_STORE_CTX_get_current_cert(store);
        int  depth = X509_STORE_CTX_get_error_depth(store);
        int  err = X509_STORE_CTX_get_error(store);
 
        cwmp_log_error("Error with certificate at depth: %i\n", depth);
        X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
        cwmp_log_error("issuer   = %s\n", data);
        X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
        cwmp_log_error("subject  = %s\n", data);
        cwmp_log_error("err %i:%s\n", err, X509_verify_cert_error_string(err));
    }
 
    return ok;
}

*/

//SSL_CTX *openssl_setup_client_ctx(const char * cafile, const char * password) 
//{
//    SSL_CTX *ctx;
//    
//    if (!SSL_library_init()) {
//        fprintf(stderr, "OpenSSL initialization failed!\n");
//        cwmp_log_error("OpenSSL initialization failed!\n");
//        exit(-1);
//    }
//    SSL_load_error_strings();
// 
//    ctx = SSL_CTX_new(SSLv3_client_method());
//    
//    /* Load our keys and certificates*/
//    if(!(SSL_CTX_use_certificate_chain_file(ctx, cafile)))
//    {
//      cwmp_log_error("Can't read certificate file");
//      
//      //return NULL;
//    }
// 
//    strncpy(openssl_password, password, 32);
// 
//    SSL_CTX_set_default_passwd_cb(ctx, openssl_password_cb);
//    if(!(SSL_CTX_use_PrivateKey_file(ctx,
//      cafile,SSL_FILETYPE_PEM)))
//    {
//      cwmp_log_error("Can't read key file");
//      //return NULL
//    }
//
//    /* Load the CAs we trust*/
//    /*if(!(SSL_CTX_load_verify_locations(ctx, CA_LIST,0)))
//    {
//      cwmp_log_error("Can't read CA list");
//      //return NULL
//    }
//    */
//    SSL_CTX_set_verify_depth(ctx,4);
//    SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
//    
//    
///*     
//    
//    if (SSL_CTX_load_verify_locations(ctx, cafile, cadir) != 1)
//        cwmp_log_error("Error loading CA file and/or directory");
//    if (SSL_CTX_set_default_verify_paths(ctx) != 1) 
//        cwmp_log_error("Error loading default CA file and/or directory");
//        
//    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, openssl_verify_callback);
//    SSL_CTX_set_verify_depth(ctx, 4);
//    SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
//    if (SSL_CTX_set_cipher_list(ctx, CIPHER_LIST) != 1) 
//        cwmp_log_error("Error setting cipher list (no valid ciphers)");
//        
//*/        
//    cwmp_log_debug("init openssl success.\n");
//    return ctx;
//}



int openssl_check_cert(SSL *ssl, char *host)  
  {
    X509 *peer;
    char peer_CN[256];
    
    if(SSL_get_verify_result(ssl)!=X509_V_OK)
    {
      cwmp_log_error("Certificate doesn't verify");
      //return CWMP_ERROR;
    }

    /*Check the cert chain. The chain length
      is automatically checked by OpenSSL when
      we set the verify depth in the ctx */

    /*Check the common name*/
    peer=SSL_get_peer_certificate(ssl);
    X509_NAME_get_text_by_NID
      (X509_get_subject_name(peer),
      NID_commonName, peer_CN, 256);
    if(strcasecmp(peer_CN,host))
    {
       cwmp_log_error("Common name doesn't match host name");
       //return CWMP_ERROR;
    }
    
    return CWMP_OK;
    
  }
  

//int http_socket_ssl_create(http_socket_t **news, SSL_CTX *ctx, pool_t * pool)
//{
//    int stat;
//    stat = http_socket_calloc(news, pool);
//    if (stat == CWMP_ERROR)
//    {
//        return CWMP_ERROR;
//    }
//    
//    if(! (*news)->use_ssl)
//    { 
//        return CWMP_ERROR;    
//    }
//   
//    (*news)->ssl = SSL_new(ctx);
//    return CWMP_OK;
//}
//



#endif

