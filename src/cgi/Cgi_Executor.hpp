/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi_Executor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:35:59 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/15 16:47:47 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"

class	Cgi_Controller;

class Cgi_Executor
{
  public:
	Cgi_Executor(Cgi_Controller *corresponding_controller);
	Cgi_Executor(const Cgi_Executor &other);
	Cgi_Executor &operator=(const Cgi_Executor &other);
	virtual ~Cgi_Executor(void);

	void start_cgi();
	void init_env_map();
	void write_request_body_to_pipe();
	void run_script();
	void env_map_to_env_arr();

	Cgi_Controller *corresponding_controller;
	int pipe_write_request_body_to_cgi[2];
	std::map<std::string, std::string> env_map;
	std::vector<char *> env_arr;

	std::string body;

	// write http headers as env vars, not all, only selected
	// check if file exists
	// check if have access rights exec

	class CgiExecutorSystemFunctionFailed : public std::exception
	{
		public:
		CgiExecutorSystemFunctionFailed(std::string function_name);
		virtual ~CgiExecutorSystemFunctionFailed() throw();
		virtual const char *what() const throw();

		private:
		std::string function_name;
		mutable std::string _msg;
	};

  private:
};
