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
#include "Cgi_Executor.hpp"
#include "webserv.hpp"

class Cgi_Controller
{
  public:
	Cgi_Controller(void);
	Cgi_Controller(const Cgi_Controller &other);
	Cgi_Controller &operator=(const Cgi_Controller &other);
	virtual ~Cgi_Controller(void);

	void start_cgi();
	bool check_cgi_executor_timeout();
	bool check_cgi_executor_finished();
	void read_cgi_executor_response();

	std::string get_random_string(size_t length);

	int executor_pid_id;
	int pipe_receive_cgi_answer[2];
	std::basic_string<unsigned char> child_response_buffer;
	bool child_response_fully_received;
	bool child_finished;
	time_t executor_start_time;
	std::string tmp_file_name;

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
