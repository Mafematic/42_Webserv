/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi_Controller.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:35:59 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/15 16:47:47 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
// #include "Client.hpp"
// #include "Request.hpp"
// #include "Server.hpp"
#include "webserv.hpp"
#include "Cgi_Executor.hpp"

class Cgi_Controller
{
  public:
	Cgi_Controller(void);
	Cgi_Controller(const Cgi_Controller &other);
	Cgi_Controller &operator=(const Cgi_Controller &other);
	virtual ~Cgi_Controller(void);

	void start_cgi();

	// void create_pipes(void);

	// start executor
	// create pipe_write_to_child
	// create pipe_read_from_child
	// close pipe_write_to_child
	// close pipe_read_from_child
	// fork execute
	// check timeout
	// kill cgi
	// send response on cgi killed

	int executor_pid_id;
	// int pipe_write_to_child[2];
	int pipe_receive_cgi_answer[2];
	std::basic_string<unsigned char> child_response_buffer;
	bool child_response_fully_received;
	time_t executor_start_time;

	// Request &corresponding_request;
	// Server &corresponding_server;
	// Client &corresponding_client;

	class CgiControllerSystemFunctionFailed : public std::exception
	{
		public:
		CgiControllerSystemFunctionFailed(std::string function_name);
		virtual ~CgiControllerSystemFunctionFailed() throw();
		virtual const char *what() const throw();

		private:
		std::string _function_name;
		mutable std::string _msg;
	};

  private:
};
