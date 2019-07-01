#include "Tintin_reporter.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#include <csignal>
#include <string>
#include <iostream>
#include <sstream>

int users = 0;
Tintin_reporter logger;

int md_fork(int sock, int daemonPid)
{
	int len;
	pid_t child;
	char buf[1024];
	int cs_socket;
	unsigned int c_len;
	struct sockaddr_in c_sin;
	std::string msg;

	if ((cs_socket = accept(sock, (struct sockaddr *)&c_sin, &c_len)) < 0)
		return (-1);
	if (users == 3)
		close(cs_socket);
	users++;
	if ((child = fork()) == 0)
	{
		while (1)
		{
			memset(buf, 0, 1024);
			if ((len = recv(cs_socket, buf, 1023, 0)) < 0)
			{
				close(cs_socket);
				exit(0);
			}
			else if (len == 0)
			{
				close(cs_socket);
			}
			else if (strcmp(buf, "quit\n") == 0)
			{
				logger.info("Request quit.\n");
				logger.info("Quitting.\n");
				kill(daemonPid, SIGKILL);
				exit(0);
			}
			else if (buf[0] != '\n')
			{
				msg = "User input: ";
				msg += buf;
				if (msg.c_str()[msg.length() -1] != '\n')
					msg += "\n";
				logger.log(msg);
			}
			buf[len] = '\0';
		}
	}
	else if (child > 0)
	{
		close(cs_socket);
	}
	return (1);
}

int createServer(void)
{
	int one;
	int sock;
	struct protoent *proto;
	struct sockaddr_in sin;

	one = 1;
	logger.info("Matt_daemon: Creating server.\n");
	proto = getprotobyname("tcp");
	if (proto == 0)
		exit(-1);
	if ((sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0)
		exit(-1);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)
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

void signal_handler(int sig)
{

	if (sig == SIGCHLD)
	{
		if (users == 3)
			logger.info(": User refused\n");
		users--;
		wait(0);
	}
}

void checkPermission(void)
{
	if (getuid() != 0)
	{
		std::cerr << "Matt_daemon: Permission denied\n"
				  << std::endl;
		exit(1);
	}
}

int main(void)
{
	int sockfd;
	int daemonPid;

	// checkPermission();
	logger.info("Matt_daemon: Started.\n");
	if (std::signal(SIGCHLD, signal_handler) == SIG_ERR)
		exit(2);
	if (open("matt_daemon.lock", O_CREAT, O_RDWR, 0644) < 0)
	{
		std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
		logger.info("Matt_daemon: Quitting.\n");
		exit(2);
	}
	sockfd = createServer();
	logger.info("Matt_daemon: Server created.\n");
	if (daemon(1, 1) < 0)
		return (-1);
	daemonPid = getpid();
	logger.info("Matt_daemon: Entering Daemon mode.\n");
	logger.info("Matt_daemon: started. PID: " + std::to_string(daemonPid) + "\n");
	while (1)
	{
		md_fork(sockfd, daemonPid);
	}
}