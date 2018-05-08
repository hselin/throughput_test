
LIBS =


all : client_send client_recv server_send server_recv

client_recv : client_recv.o
	gcc -Wall client_recv.o -o client_recv $(LIBS)

client_send : client_send.o
	gcc -Wall client_send.o -o client_send $(LIBS)

server_recv : server_recv.o
	gcc -Wall server_recv.o -o server_recv $(LIBS)

server_send : server_send.o
	gcc -Wall server_send.o -o server_send $(LIBS)

client_recv.o : client_recv.c
	gcc -Wall -c -D_FILE_OFFSET_BITS=64 client_recv.c

client_send.o : client_send.c
	gcc -Wall -c -D_FILE_OFFSET_BITS=64 client_send.c

server_recv.o : server_recv.c
	gcc -Wall -c -D_FILE_OFFSET_BITS=64 server_recv.c

server_send.o : server_send.c
	gcc -Wall -c -D_FILE_OFFSET_BITS=64 server_send.c

clean:
	rm -f *.o *~ client_send client_recv server_send server_recv
