/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysan-seb <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/05 11:47:36 by ysan-seb          #+#    #+#             */
/*   Updated: 2019/07/05 11:47:38 by ysan-seb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <ctime>
#include <iostream>
#include <fstream>
#include <locale>
#include <fcntl.h>

class Tintin_reporter
{
	public:
		Tintin_reporter(void);
		Tintin_reporter(Tintin_reporter const &src);
		~Tintin_reporter();
		Tintin_reporter &operator=(Tintin_reporter const &rhs);

		std::ofstream getFile(void);

		void log(std::string msg);
		void info(std::string msg);
		void error(std::string msg);

	private:
		void logtime(void);
		std::time_t time;
		std::ofstream file;
		char timeFormat[26];
};

#endif
