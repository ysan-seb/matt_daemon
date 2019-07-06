/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maki <maki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 11:51:30 by ysan-seb          #+#    #+#             */
/*   Updated: 2019/07/06 22:40:26 by maki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter(void)
{
	this->file.open ("/var/log/matt_daemon/matt_daemon.log", std::fstream::app | std::fstream::out);
}

Tintin_reporter::Tintin_reporter(Tintin_reporter const &src)
{
	*this = src;
}

Tintin_reporter::~Tintin_reporter(void) {
	this->file.close();
}

Tintin_reporter & Tintin_reporter::operator=(Tintin_reporter const &rhs)
{
	(void)rhs;
	return *this;
}

void Tintin_reporter::logtime(void)
{
	this->time = std::time(0);
	if (std::strftime(this->timeFormat, sizeof(this->timeFormat),
				"[%d/%m/%Y-%H:%M:%S]", std::localtime(&this->time)) == 0)
	{
		this->timeFormat[0] = '\0';
	}
}

void Tintin_reporter::log(std::string msg)
{
	if (this->file.is_open()) {
		this->logtime();
		this->file << this->timeFormat << " [ LOG ] - " << msg;
		this->file.flush();
	}
}

void Tintin_reporter::info(std::string msg)
{
	if (this->file.is_open()) {
		this->logtime();
		this->file << this->timeFormat << " [ INFO ] - " << msg;
		this->file.flush();
	}
}

void Tintin_reporter::error(std::string msg)
{
	if (this->file.is_open()) {
		this->logtime();
		this->file << this->timeFormat << " [ ERROR ] - " << msg;
		this->file.flush();
	}
}
