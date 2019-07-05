/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysan-seb <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 11:48:03 by ysan-seb          #+#    #+#             */
/*   Updated: 2019/07/05 11:48:46 by ysan-seb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/matt_daemon.hpp"

int users;
Tintin_reporter logger;
std::vector<int> pids;
int daemonPid;

int default_action(int sig)
{
	if (sig != SIGCHLD && sig != SIGCONT && sig != SIGURG && sig != SIGWINCH)
		return (TERMINATE);
	return (IGNORE);
}

void sigchld(int sig)
{
	int pid;
	int status;

	status = 0;
	if (sig == SIGCHLD)
	{
		pid = waitpid(0, &status, WNOHANG);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 42)
		{
			for (unsigned long i = 0; i < pids.size(); i++) {
				if (pids[i] == pid)
					pids.erase(pids.begin() + i);
			}
			users--;
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == 84)
		{
			for (unsigned long i = 0; i < pids.size(); i++)
				kill(pids[i], SIGKILL);
			if (remove("/var/lock/matt_daemon.lock") != 0)
				logger.error("Matt_daemon: error while deleting the file");
			logger.info("Request quit.\n");
			logger.info("Quitting.\n");
			kill(daemonPid, SIGKILL);
		}
	}
}

void signal_handler(int sig)
{
	std::string signame;

	signame = strsignal(sig);
	logger.info("Matt_daemon: Signal handler (" + signame + ").\n");
	if (default_action(sig) == TERMINATE)
	{
		for (unsigned long i = 0; i < pids.size(); i++)
			kill(pids[i], SIGKILL);
		logger.info("Matt_daemon: Quitting.\n");
		exit(EXIT_SUCCESS);
	}
	else
	{
		sigchld(sig);
	}
}

void signals(void)
{
	for (int sig = 1; sig < 32; sig++)
	{
		if (sig != SIGKILL && sig != SIGSTOP && signal(sig, signal_handler) == SIG_ERR)
			exit(EXIT_FAILURE);
	}
}
