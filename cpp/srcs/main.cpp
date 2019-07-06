/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maki <maki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 11:48:49 by ysan-seb          #+#    #+#             */
/*   Updated: 2019/07/06 21:43:22 by maki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/matt_daemon.hpp"

extern int users;
extern Tintin_reporter logger;
extern std::vector<int> pids;
extern int daemonPid;

void child_process(int cs_socket)
{
	int len;
	char buf[1024];
	std::string msg;
	
	while (1)
	{
		memset(buf, 0, 1024);
		if ((len = recv(cs_socket, buf, 1023, 0)) < 0)
		{
			close(cs_socket);
			exit(42);
		}
		else if (len == 0)
			close(cs_socket);
		else if (strcmp(buf, "quit\n") == 0)
			exit(84);
		else if (buf[0] != '\n')
		{
			msg = "Matt_daemon: User input: ";
			msg += buf;
			if (msg.c_str()[msg.length() - 1] != '\n')
				msg += "\n";
			logger.log(msg);
		}
		buf[len] = '\0';
	}
}

int md_fork(int sock)
{
	int child;

	int cs_socket;
	unsigned int c_len;
	struct sockaddr_in c_sin;

	if ((cs_socket = accept(sock, (struct sockaddr *)&c_sin, &c_len)) < 0)
	{
		logger.error("Matt_daemon: Error durring acceptation client.\n");
		exit (-1);
	}
	if (users == 3) {
		close(cs_socket);
		return (1);
	}
	users++;
	if ((child = fork()) == 0)
	{
		close(sock);
		child_process(cs_socket);
	}
	else if (child > 0)
	{
		pids.push_back(child);
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
		exit(EXIT_FAILURE);
	if ((sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0)
		exit(EXIT_FAILURE);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)
		exit(EXIT_FAILURE);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4242);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (const struct sockaddr *)&sin, sizeof(sin)) < 0)
		exit(EXIT_FAILURE);
	if (listen(sock, 3) != 0)
		exit(EXIT_FAILURE);
	return (sock);
}

void checkPermission(void)
{
	if (getuid() != 0)
	{
		std::cerr << "Matt_daemon: Permission denied, you must be root." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void _daemon(void)
{
	pid_t pid;
	pid_t sid;

	pid = fork();
	if (pid < 0)
		exit(1);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	chdir("/");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(void)
{
	int file;
	int sockfd;
	
	checkPermission();
	users = 0;
	logger.info("Matt_daemon: Started.\n");
	mkdir("/var/log/matt_daemon/", 0700);
	signals();
	if ((file = open("/var/lock/matt_daemon.lock", O_CREAT | O_RDWR, 0644)) < 0)
	{
		printf("%s\n", strerror(errno));
		std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
		logger.info("Matt_daemon: Quitting.\n");
		exit(EXIT_FAILURE);
	}
	if (flock(file, LOCK_EX | LOCK_NB) < 0)
	{
		std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
		logger.error("Matt_daemon: Error file locked.\n");
		logger.info("Matt_daemon: Quitting.\n");
		exit(EXIT_FAILURE);
	}
	sockfd = createServer();
	logger.info("Matt_daemon: Server created.\n");
	_daemon();
	daemonPid = getpid();
	logger.info("Matt_daemon: Entering Daemon mode.\n");
	logger.info("Matt_daemon: started. PID: " + std::to_string(daemonPid) + "\n");
	while (1)
	{
		md_fork(sockfd);
	}
	return (0);
}
