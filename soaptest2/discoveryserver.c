#include "soapH.h"
#include "wsdd.nsmap"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define __DEBUG
#ifdef __DEBUG
#define DBG(fmt,args...) fprintf(stdout,  fmt,  ##args)
#else
#define DBG(fmt,args...)
#endif
#define ERR(fmt,args...) fprintf(stderr,  fmt,  ##args)


#define set_block_fd(fd)    fcntl(fd, F_SETFL, 0)
#define set_nonblock_fd(fd) fcntl(fd, F_SETFL, NOBLOCK)

char *server_ip="192.168.1.119";

#define SMALL_INFO_LENGTH 20
#define IP_LENGTH 20
#define INFO_LENGTH 100

int bind_server_udp1(int server_s)
{
	struct sockaddr_in local_addr;
	memset(&local_addr,0,sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(3702);
	return bind(server_s,(struct sockaddr*)&local_addr,sizeof(local_addr));

}

static int create_server_socket_udp(void)
{
	int server_udp;
	unsigned char one = 1;
	int sock_opt = 1;

	server_udp = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (server_udp == -1) {
        printf("unable to create socket\n");
    }

    /* reuse socket addr */
    if ((setsockopt(server_udp, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
                    sizeof (sock_opt))) == -1) {
        printf("setsockopt\n");
    }
    if ((setsockopt(server_udp, IPPROTO_IP, IP_MULTICAST_LOOP,
                       &one, sizeof (unsigned char))) == -1) {
        printf("setsockopt\n");
    }

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(server_udp,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))==-1)
	{
		perror("memberchip error\n");
	}

	return server_udp;
}

int main()
{
	int server_udp;

	int retval=0;
	struct soap *soap_udp;
	int fault_flag=0;
	
	server_udp = create_server_socket_udp();
	bind_server_udp1(server_udp);

	while(1){
		soap_udp=soap_new();
		soap_init1(soap_udp, SOAP_IO_UDP);
		soap_udp->master = server_udp;
		soap_udp->socket = server_udp;
		soap_udp->errmode = 0;
		soap_udp->bind_flags = 1;
		if (!soap_valid_socket(soap_bind(soap_udp, NULL, 3702, 100)))
		{	 
			soap_print_fault(soap_udp, stderr);
		}
		fprintf(stderr,"soap_serve starting..\n");
		retval = soap_serve(soap_udp); //×èÈûÔÚÕâÀï
		fprintf(stderr,"retval=%d\n",retval);
		if(retval && !(fault_flag))
		{
			fault_flag = 1;
		}
		else if(!retval)
		{
			fault_flag = 0;
		}
		soap_destroy(soap_udp);
		soap_end(soap_udp);
		soap_done(soap_udp);
		free(soap_udp);
	}
}

int main_test(int argc, char **argv)  
{  
    int m, s;  
    struct soap add_soap;  
    soap_init(&add_soap);  
    soap_set_namespaces(&add_soap, namespaces);  
  
    if (argc < 2) {  
        printf("usage: %s <server_port> \n", argv[0]);  
        exit(1);  
    } else {  
        m = soap_bind(&add_soap, NULL, atoi(argv[1]), 100);  
        if (m < 0) {  
            soap_print_fault(&add_soap, stderr);  
            exit(-1);  
        }  
        fprintf(stderr, "Socket connection successful: master socket = %d\n", m);  
        for (;;) {  
            s = soap_accept(&add_soap);  
            if (s < 0) {  
                soap_print_fault(&add_soap, stderr);  
                exit(-1);  
            }  
            fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);  
            soap_serve(&add_soap);  
            soap_end(&add_soap);  
        }  
    }  
    return 0;  
} 

int onvif_fault(struct soap *soap,char *value1,char *value2)
{
	soap->fault = (struct SOAP_ENV__Fault*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Fault)));
	soap->fault->SOAP_ENV__Code = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Sender";
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value = value1;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Value = value2;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Subcode = NULL;
	soap->fault->faultcode = NULL;
	soap->fault->faultstring = NULL;
	soap->fault->faultactor = NULL;
	soap->fault->detail = NULL;
	soap->fault->SOAP_ENV__Reason = NULL;
	soap->fault->SOAP_ENV__Node = NULL;
	soap->fault->SOAP_ENV__Role = NULL;
	soap->fault->SOAP_ENV__Detail = NULL;
}

int  __wsdd__Hello(struct soap* soap, struct wsdd__HelloType *wsdd__Hello)
{
	fprintf(stderr,"__wsdd__Hello\n");
}

int  __wsdd__Bye(struct soap* soap, struct wsdd__ByeType *wsdd__Bye)
{
	fprintf(stderr,"__wsdd__Bye\n");
}













SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ProbeMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{	struct __wsdd__ProbeMatches soap_tmp___wsdd__ProbeMatches;
	if (!soap_action)
		soap_action = "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ProbeMatches";
	soap->encodingStyle = NULL;
	soap_tmp___wsdd__ProbeMatches.wsdd__ProbeMatches = wsdd__ProbeMatches;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int  __wsdd__Probe(struct soap* soap, struct wsdd__ProbeType *wsdd__Probe) 
{  
    DBG("__wsdd__Probe\n");  
    char macaddr[6];  
    char _IPAddr[INFO_LENGTH];  
    char _HwId[1024];  
      
    wsdd__ProbeMatchesType ProbeMatches;  
    ProbeMatches.ProbeMatch = (struct wsdd__ProbeMatchType *)soap_malloc(soap, sizeof(struct wsdd__ProbeMatchType));  
    ProbeMatches.ProbeMatch->XAddrs = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);  
    ProbeMatches.ProbeMatch->Types = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);  
    ProbeMatches.ProbeMatch->Scopes = (struct wsdd__ScopesType*)soap_malloc(soap,sizeof(struct wsdd__ScopesType));  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties = (struct wsa__ReferencePropertiesType*)soap_malloc(soap,sizeof(struct wsa__ReferencePropertiesType));  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters = (struct wsa__ReferenceParametersType*)soap_malloc(soap,sizeof(struct wsa__ReferenceParametersType));  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName = (struct wsa__ServiceNameType*)soap_malloc(soap,sizeof(struct wsa__ServiceNameType));  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType = (char **)soap_malloc(soap, sizeof(char *) * SMALL_INFO_LENGTH);  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__any = (char **)soap_malloc(soap, sizeof(char*) * SMALL_INFO_LENGTH);  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__anyAttribute = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);  
  
    macaddr[0]=0x01;macaddr[1]=0x01;macaddr[2]=0x01;macaddr[3]=0x01;macaddr[4]=0x01;macaddr[5]=0x01;  
    sprintf(_HwId,"urn:uuid:2419d68a-2dd2-21b2-a205-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);  
  
    sprintf(_IPAddr, "http://%03d.%03d.%1d.%03d/onvif/device_service", 192, 168, 1, 233);  
    ProbeMatches.__sizeProbeMatch = 1;  
    ProbeMatches.ProbeMatch->Scopes->__item =(char *)soap_malloc(soap, 1024);  
    memset(ProbeMatches.ProbeMatch->Scopes->__item,0,sizeof(ProbeMatches.ProbeMatch->Scopes->__item));    
  
    //Scopes MUST BE  
    strcat(ProbeMatches.ProbeMatch->Scopes->__item, "onvif://www.onvif.org/type/NetworkVideoTransmitter");  
  
    ProbeMatches.ProbeMatch->Scopes->MatchBy = NULL;  
    strcpy(ProbeMatches.ProbeMatch->XAddrs, _IPAddr);  
    strcpy(ProbeMatches.ProbeMatch->Types, wsdd__Probe->Types);  
    DBG("wsdd__Probe->Types=%s\n",wsdd__Probe->Types);  
    ProbeMatches.ProbeMatch->MetadataVersion = 1;  
    //ws-discovery规定 为可选项  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties->__size = 0;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties->__any = NULL;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters->__size = 0;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters->__any = NULL;  
      
    ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);  
    //ws-discovery规定 为可选项  
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType[0], "ttl");  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->__item = NULL;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->PortName = NULL;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->__anyAttribute = NULL;  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__any[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);  
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.__any[0], "Any");  
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.__anyAttribute, "Attribute");  
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__size = 0;  
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.Address, _HwId);  
  
    /*注释的部分为可选，注释掉onvif test也能发现ws-d*/  
    //soap->header->wsa__To = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";  
    //soap->header->wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";  
    soap->header->wsa__RelatesTo = (struct wsa__Relationship*)soap_malloc(soap, sizeof(struct wsa__Relationship));  
    //it's here  
    soap->header->wsa__RelatesTo->__item = soap->header->wsa__MessageID;  
    soap->header->wsa__RelatesTo->RelationshipType = NULL;  
    soap->header->wsa__RelatesTo->__anyAttribute = NULL;  
  
    soap->header->wsa__MessageID =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);  
    strcpy(soap->header->wsa__MessageID,_HwId+4);  
  
    /* send over current socket as HTTP OK response: */  
    /*测试过，第二参数必须http，action随意*/  
    soap_send___wsdd__ProbeMatches(soap, "http://", NULL, &ProbeMatches);  
    return SOAP_OK;  
  
}  


int  __wsdd__ProbeMatches(struct soap* soap, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{
	fprintf(stderr,"__wsdd__ProbeMatches\n");
}
int  __wsdd__Resolve(struct soap* soap, struct wsdd__ResolveType *wsdd__Resolve)
{
	fprintf(stderr,"__wsdd__Resolve\n");
}
int  __wsdd__ResolveMatches(struct soap* soap, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
	fprintf(stderr,"__wsdd__ResolveMatches\n");
}
int  __ns1__Hello(struct soap* soap, struct wsdd__HelloType tdn__Hello, struct wsdd__ResolveType *tdn__HelloResponse)
{
	fprintf(stderr,"__ns1__Hello\n");
}
int  __ns1__Bye(struct soap* soap, struct wsdd__ByeType tdn__Bye, struct wsdd__ResolveType *tdn__ByeResponse)
{
	fprintf(stderr,"__ns1__Bye\n");
}
int  __ns2__Probe(struct soap* soap, struct wsdd__ProbeType tdn__Probe, struct wsdd__ProbeMatchesType *tdn__ProbeResponse)
{
	fprintf(stderr,"__ns2__Probe\n");
}



















