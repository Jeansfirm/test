CC = gcc -g -DWITH_NONAMESPACES
SERVER_OBJS = soapC.o stdsoap2.o soapServer.o addServer.o
CLIENT_OBJS = soapC.o stdsoap2.o soapClient.o addClient.o addtest.o

all:server
server:$(SERVER_OBJS)
	$(CC) -o addserver $(SERVER_OBJS)

client:$(CLIENT_OBJS)
	$(CC) -o addtest $(CLIENT_OBJS)

clean:
	rm -f *.o addtest
