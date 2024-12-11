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
#include "Client.hpp"

class Cgi_Controller
{
  public:
	Cgi_Controller();
	Cgi_Controller(Client client);
	Cgi_Controller(const Cgi_Controller &other);
	Cgi_Controller &operator=(const Cgi_Controller &other);
	virtual ~Cgi_Controller(void);

	void start_cgi();
	e_cgi_status check_cgi();

	int pipe_receive_cgi_answer[2];
	std::string tmp_file_name;
	Client	corresponding_client;

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
	int executor_pid_id;
	e_cgi_status status;
	time_t executor_start_time;

	std::string get_random_string(size_t length);
	bool check_cgi_executor_timeout();
	bool check_cgi_executor_finished();
};
