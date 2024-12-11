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
#include "Client.hpp"
#include "Request.hpp"
#include "Route.hpp"
#include "Server.hpp"
#include "webserv.hpp"
#include "Path_Analyser.hpp"

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
	void add_http_headers_to_env_map();
	void put_request_body_into_stdin();
	void run_script();
	void env_map_to_env_arr();
	void create_argv_arr();
	void analyse_path();

	Cgi_Controller *corresponding_controller;

	std::map<std::string, std::string> env_map;
	char **env_arr;
	char **argv_arr;
	std::string body;
	// write http headers as env vars, not all, only selected

	class CgiExecutorSystemFunctionFailed : public std::exception
	{
		public:
		CgiExecutorSystemFunctionFailed(std::string function_name);
		virtual ~CgiExecutorSystemFunctionFailed() throw();
		virtual const char *what() const throw();

		private:
		std::string _function_name;
		mutable std::string _msg;
	};

  private:
	Client _corresponding_client;
	Server _corresponding_server;
	Request _corresponding_request;
	Route _corresponding_route;
	Path_Analyser _path_analyser;
};
