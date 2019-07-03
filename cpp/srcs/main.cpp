#include "../inc/matt_daemon.hpp"

extern int users;
extern Tintin_reporter logger;
extern std::vector<int> pids;

int md_fork(int sock, int daemonPid)
{
	int len;
	int child;

	char buf[1024];
	int cs_socket;
	unsigned int c_len;
	struct sockaddr_in c_sin;
	std::string msg;


//
	printf("Users : %d\n", users);
	if ((cs_socket = accept(sock, (struct sockaddr *)&c_sin, &c_len)) < 0)
		return (-1);
	if (users == 3) {
		close(cs_socket);
		return (1);
	}
	users++;
	if ((child = fork()) == 0)
	{
		close(sock);
		while (1)
		{
			memset(buf, 0, 1024);
			if ((len = recv(cs_socket, buf, 1023, 0)) < 0)
			{
				close(cs_socket);
				exit(42);
			}
			else if (len == 0)
			{
				close(cs_socket);
			}
			else if (strcmp(buf, "quit\n") == 0)
			{
				logger.info("Request quit.\n");
				logger.info("Quitting.\n");

				for (unsigned long i = 0; i < pids.size(); i++)
				{
					if (pids[i] != child)
						kill(pids[i], SIGKILL);
				}
				kill(child, SIGKILL);
				if (remove("matt_daemon.lock") != 0)
					perror("Error deleting file");
				kill(daemonPid, SIGKILL);
				exit(0);
			}
			else if (buf[0] != '\n')
			{
				msg = "User input: ";
				msg += buf;
				if (msg.c_str()[msg.length() - 1] != '\n')
					msg += "\n";
				logger.log(msg);
			}
			buf[len] = '\0';
		}
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

void checkPermission(void)
{
	if (getuid() != 0)
	{
		std::cerr << "Matt_daemon: Permission denied\n"
				  << std::endl;
		exit(1);
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
		exit(0);
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(1);
	chdir("/");
	// close(STDIN_FILENO);
	// close(STDOUT_FILENO);
	// close(STDERR_FILENO);
}

//https://github.com/torvalds/linux/blob/master/include/linux/signal.h

int main(void)
{
	int file;
	int sockfd;
	int daemonPid;

	// checkPermission();
	users = 0;
	logger.info("Matt_daemon: Started.\n");
	signals();
	if ((file = open("matt_daemon.lock", O_CREAT | O_RDWR, 0644)) < 0)
	{
		std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
		logger.info("Matt_daemon: Quitting.\n");
		exit(2);
	}
	if (flock(file, LOCK_EX | LOCK_NB) < 0)
	{
		std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
		logger.error("Matt_daemon: Error file locked.\n");
		printf("[%d] %s\n", errno, strerror(errno));
		logger.info("Matt_daemon: Quitting.\n");
		exit(1);
	}
	sockfd = createServer();
	logger.info("Matt_daemon: Server created.\n");
	_daemon();
	daemonPid = getpid();
	logger.info("Matt_daemon: Entering Daemon mode.\n");
	logger.info("Matt_daemon: started. PID: " + std::to_string(daemonPid) + "\n");
	while (1)
	{
		md_fork(sockfd, daemonPid);
	}
}