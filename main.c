#include <unistd.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int		users;

void		showtime()
{
	time_t 		t;
    	char 		buffer[26];
    	struct tm	*tm_info;

   	time(&t);
    	tm_info = localtime(&t);
    	strftime(buffer, 26, "[%d/%m/%Y-%H:%M:%S]", tm_info);
   	write(1, buffer, strlen(buffer));
}

void		signal_handler(int sig)
{
	if (sig == SIGCHLD)
	{
		users--;
		showtime();
		printf(": User disconected\n");
		wait(0);
	}
}

int		md_fork(int sock)
{
	int			len;
	pid_t			child;
	char			buf[1024];
	int			cs_socket;
	unsigned int		c_len;
	struct sockaddr_in	c_sin;

	if ((cs_socket = accept(sock, (struct sockaddr*)&c_sin, &c_len)) < 0)
		return (-1);	
	users++;
	showtime();
	printf(": User connection\n");
	if ((child = fork()) == 0)
	{
		while (1)
		{
			memset(buf, 0, 1024);
			if ((len = recv(cs_socket, buf, 1023, 0)) <= 0)
			{
				close(cs_socket);
				exit(0);
			}
			buf[len] = '\0';
		}
	} else if (child < 0) {
		printf("ERROR\n");
	}
	return (1);
}

int	create_server(void)
{
	int			sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;


	showtime();
	printf(" [ INFO ] - Matt_daemon: Creating server.\n");
	proto = getprotobyname("tcp");
	if (proto == 0)
		 exit(-1);
	if ((sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0)
		exit(-1);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
		exit(-1);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4242);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (const struct sockaddr *)&sin, sizeof(sin)) < 0)
		exit(-1);
	if (listen(sock, 3) != 0)
		exit(-1);
	return (sock); 
}

int	main(void)
{
	int	ret;
	int	sockfd;
	char	buf[1024];

	if (getuid() != 0)
		dprintf(2, "%s: Permissions denied\n", __FILE__);
	showtime();
	printf(" [ INFO ] - Matt_daemon: Started.\n");
	if (signal(SIGCHLD, signal_handler) == SIG_ERR)
		exit(2);
	sockfd = create_server();
	showtime();
	printf(" [ INFO ] - Matt_daemon: Server created.\n");
	if (daemon(1, 1) < 0)
		return (-1);
	showtime();
	printf(" [ INFO ] - Matt_daemon: Entering Daemon mode.\n");
	showtime();
	printf(" [ INFO ] - Matt_daemon: started. PID: %d.\n", getpid());
	while(1)
	{
		md_fork(sockfd);
	}
}
