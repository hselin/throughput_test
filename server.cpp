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

#define NUM_CONNECTIONS			(20)
#define SERV_PORT				(9090)


#define KB						(1024)
#define MB						(1024 * KB)
#define GB						(1024 * MB)

#define BUFFER_SIZE				(1 * MB)

#define TEST_TIME_US			(10 * 1000000)

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

uint64_t getElapsedTime(struct timespec *start, struct timespec *end)
{
    return ((end->tv_sec - start->tv_sec) * 1000000) + ((end->tv_nsec - start->tv_nsec) / 1000);
}

int main(int argc, char **argv)
{
	int listen_fd;
	struct sockaddr_in addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERV_PORT);

	bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr));
	listen(listen_fd, NUM_CONNECTIONS);

	int fds[NUM_CONNECTIONS];

	for(int i = 0; i < NUM_CONNECTIONS; i++)
	{
		struct sockaddr_in remote_addr;
		socklen_t addr_len = sizeof(remote_addr);

		fds[i] = accept(listen_fd, (struct sockaddr *) &remote_addr, &addr_len);

		printf("accepted: %d\n", i);

		set_non_block(fds[i]);
	}



	struct timespec start, now;
	uint64_t elapsedTime;

	char buffer[BUFFER_SIZE];
	uint64_t total_bytes_received = 0;

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	do
	{
		for(int i = 0; i < NUM_CONNECTIONS; i++)
		{
			ssize_t status = recv(fds[i], buffer, BUFFER_SIZE, MSG_DONTWAIT);

			if(status > 0)
				total_bytes_received += status;
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &now);
		elapsedTime = getElapsedTime(&start, &now);
	} while(elapsedTime <= TEST_TIME_US);

	printf("total_bytes_received: %lu\n", total_bytes_received);
	printf("throughput: %fMB/s\n", ((float)total_bytes_received / (float)MB) / (elapsedTime / 1000000));

	return 0;
}