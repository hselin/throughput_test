#include <iostream>
#include <cstdio>
#include <string>
#include <ostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define SERV_PORT				(9090)


#define KB						(1024)
#define MB						(1024 * KB)
#define GB						(1024 * MB)

#define BUFFER_SIZE				(1 * MB)

void set_non_block(int fd)
{
	int flags;

	/* Set socket to non-blocking */ 
	if((flags = fcntl(fd, F_GETFL, 0)) < 0) 
	{ 
	   assert(0);
	} 


	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
	{ 
	    assert(0);
	} 
}

int main(int argc, char **argv)
{
	int num_connections = atoi(argv[1]);

	int fds[num_connections];
 	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[2]);
	addr.sin_port = htons(SERV_PORT);

 	for(int i = 0; i < num_connections; i++)
 	{
 		fds[i] = socket(AF_INET, SOCK_STREAM, 0);
 		assert(fds[i] >= 0);

 		int status = connect(fds[i], (struct sockaddr *) &addr, sizeof(addr));

 		if(status < 0)
 		{
 			perror("error connecting");
 		}

 		assert(!status);

 		set_non_block(fds[i]);
 	}

 	

 	char buffer[BUFFER_SIZE];

 	while(1)
 	{
 		for(int i = 0; i < num_connections; i++)
 		{
 			write(fds[i], buffer, BUFFER_SIZE);
 		}

 		usleep(10000);
 	}

	return 0;
}