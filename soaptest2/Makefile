#GSOAP_ROOT = /root/onvif/gsoap-2.8/gsoap
#CC = gcc -g -DWITH_NONAMESPACES
CC = gcc -g
#INCLUDE = -I$(GSOAP_ROOT)
SERVER_OBJS = soapC.o stdsoap2.o soapServer.o discoveryserver.o 
CLIENT_OBJS = soapC.o stdsoap2.o soapClient.o discoveryclient.o

all: server
server: $(SERVER_OBJS) 
	$(CC) -o discoveryserver.tmp $(SERVER_OBJS) 

client: $(CLIENT_OBJS) 
	$(CC) -o discoveryclient $(CLIENT_OBJS)

clean:
	rm -f *.o discoveryserver.tmp discoveryclient
