#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <time.h>
#include <signal.h>

#define SERV_PORT				(8000)


#define KB						(1024)
#define MB						(1024 * KB)
#define GB						(1024 * MB)

#define BUFFER_SIZE				(1 * MB)

struct timespec start, now;
uint64_t elapsedTime;
uint64_t total_bytes_sent = 0;
int exit_program = 0;

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

void disable_nagle(int fd)
{
	int flag = 1;
	int result = setsockopt(fd,            /* socket affected */
							IPPROTO_TCP,     /* set option at TCP level */
							TCP_NODELAY,     /* name of option */
							(char *) &flag,  /* the cast is historical cruft */
							sizeof(int));    /* length of option value */
	assert(!result);
}

uint64_t getElapsedTime(struct timespec *start, struct timespec *end)
{
    return ((end->tv_sec - start->tv_sec) * 1000000) + ((end->tv_nsec - start->tv_nsec) / 1000);
}

void handle_sigpipe(int sig)
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	elapsedTime = getElapsedTime(&start, &now);
	printf("total_bytes_sent: %lu\n", total_bytes_sent);
	printf("throughput: %fMB/s\n", ((float)total_bytes_sent / (float)MB) / (elapsedTime / 1000000));
	exit_program = 1;
	exit(1);
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
		disable_nagle(fds[i]);
	}

	char *buffer = (char *)calloc(1, BUFFER_SIZE);
	assert(buffer);

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	signal(SIGPIPE, handle_sigpipe);

	while(1)
	{
		for(int i = 0; i < num_connections; i++)
		{
			ssize_t status = write(fds[i], buffer, BUFFER_SIZE);
			if(status > 0)
				total_bytes_sent += status;
		}
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	elapsedTime = getElapsedTime(&start, &now);
	printf("total_bytes_sent: %lu\n", total_bytes_sent);
	printf("throughput: %fMB/s\n", ((float)total_bytes_sent / (float)MB) / (elapsedTime / 1000000));

	return 0;
}