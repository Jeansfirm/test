/* soapClient.c
   Generated by gSOAP 2.7.9l from add.h
   Copyright(C) 2000-2007, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "soapH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) soapClient.c ver 2.7.9l 2015-07-29 08:28:20 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns2__add(struct soap *soap, const char *soap_endpoint, const char *soap_action, int num1, int num2, int *sum)
{	struct ns2__add soap_tmp_ns2__add;
	struct ns2__addResponse *soap_tmp_ns2__addResponse;
	soap->encodingStyle = NULL;
	soap_tmp_ns2__add.num1 = num1;
	soap_tmp_ns2__add.num2 = num2;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns2__add(soap, &soap_tmp_ns2__add);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__add(soap, &soap_tmp_ns2__add, "ns2:add", "")
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
	 || soap_put_ns2__add(soap, &soap_tmp_ns2__add, "ns2:add", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_int(soap, sum);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_ns2__addResponse = soap_get_ns2__addResponse(soap, NULL, "ns2:addResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (sum && soap_tmp_ns2__addResponse->sum)
		*sum = *soap_tmp_ns2__addResponse->sum;
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

/* End of soapClient.c */
