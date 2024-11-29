/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi_Controller.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi_Controller.hpp"

Cgi_Controller::Cgi_Controller()
	: child_response_fully_received(false)
{
	return ;
}

Cgi_Controller::Cgi_Controller(const Cgi_Controller &other)
{
	this->executor_pid_id = other.executor_pid_id;
	this->pipe_receive_cgi_answer[0] = other.pipe_receive_cgi_answer[0];
	this->pipe_receive_cgi_answer[1] = other.pipe_receive_cgi_answer[1];
	this->child_response_buffer = other.child_response_buffer;
	this->child_response_fully_received = other.child_response_fully_received;
	this->executor_start_time = other.executor_start_time;
	// this->corresponding_request = other.corresponding_request;
	// this->corresponding_server = other.corresponding_server;
	// this->corresponding_client = other.corresponding_client;
	return ;
}

Cgi_Controller &Cgi_Controller::operator=(const Cgi_Controller &other)
{
	if (this != &other)
	{
		this->executor_pid_id = other.executor_pid_id;
		this->pipe_receive_cgi_answer[0] = other.pipe_receive_cgi_answer[0];
		this->pipe_receive_cgi_answer[1] = other.pipe_receive_cgi_answer[1];
		this->child_response_buffer = other.child_response_buffer;
		this->child_response_fully_received = other.child_response_fully_received;
		this->executor_start_time = other.executor_start_time;
		// this->corresponding_request = other.corresponding_request;
		// this->corresponding_server = other.corresponding_server;
		// this->corresponding_client = other.corresponding_client;
	}
	return (*this);
}

Cgi_Controller::~Cgi_Controller(void)
{
	return ;
}

void Cgi_Controller::start_cgi()
{
	if (pipe(this->pipe_receive_cgi_answer) < 0)
		throw(CgiControllerSystemFunctionFailed("pipe"));
	this->executor_pid_id = fork();
	if (this->executor_pid_id == 0)
	{
		Cgi_Executor executor(this);
		executor.start_cgi();
	}
	else if (this->executor_pid_id > 0)
	{
		if (close(this->pipe_receive_cgi_answer[1]) < 0)
			throw(CgiControllerSystemFunctionFailed("close"));
	}
	else
		throw(CgiControllerSystemFunctionFailed("fork"));
}

Cgi_Controller::CgiControllerSystemFunctionFailed::CgiControllerSystemFunctionFailed(std::string function_name)
	: _function_name(function_name)
{
}

Cgi_Controller::CgiControllerSystemFunctionFailed::~CgiControllerSystemFunctionFailed() throw()
{
}

const char *Cgi_Controller::CgiControllerSystemFunctionFailed::what() const throw()
{
	this->_msg = "system call: ";
	this->_msg += this->_function_name;
	this->_msg = " failed inside the Cgi_Controller";
	this->_msg += "\n";
	return (this->_msg.c_str());
}