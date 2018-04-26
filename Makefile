
LIBS =


all : client server

client : client.o
	g++ -Wall client.o -o client $(LIBS)

server : server.o
	g++ -Wall server.o -o server $(LIBS)

client.o : client.cpp
	g++ -Wall -c -std=c++11 -D_FILE_OFFSET_BITS=64 client.cpp

server.o : server.cpp
	g++ -Wall -c -std=c++11 -D_FILE_OFFSET_BITS=64 server.cpp

clean:
	rm -f *.o *~ client server
