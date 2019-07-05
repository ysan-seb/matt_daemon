/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matt_daemon.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysan-seb <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 11:47:42 by ysan-seb          #+#    #+#             */
/*   Updated: 2019/07/05 11:47:44 by ysan-seb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATT_DAEMON_HPP
#define MATT_DAEMON_HPP

#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>

#include "../inc/Tintin_reporter.hpp"
#include <csignal>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>

#define TERMINATE 1
#define IGNORE 0

void	signals(void);
int		default_action(int sig);
void	signal_handler(int sig);

#endif
