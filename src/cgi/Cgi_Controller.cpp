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

Cgi_Controller::Cgi_Controller() : status(CGI_RUNNING)
{
	this->tmp_file_name = "aaa_" + this->get_random_string(32);
	return ;
}

Cgi_Controller::Cgi_Controller(const Cgi_Controller &other)
{
	this->executor_pid_id = other.executor_pid_id;
	this->pipe_receive_cgi_answer[0] = other.pipe_receive_cgi_answer[0];
	this->pipe_receive_cgi_answer[1] = other.pipe_receive_cgi_answer[1];
	this->executor_start_time = other.executor_start_time;
	this->tmp_file_name = other.tmp_file_name;
	this->status = other.status;
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
		this->executor_start_time = other.executor_start_time;
		this->tmp_file_name = other.tmp_file_name;
		this->status = other.status;
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
	char	buffer[10000];

	buffer[1] = 'A';
	buffer[0] = '\0';

	this->executor_start_time = time(NULL);
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
		// this is only for testing purposes 
		// >>
		if (close(this->pipe_receive_cgi_answer[1]) < 0)
			throw(CgiControllerSystemFunctionFailed("close"));
		while (this->status == CGI_RUNNING)
		{
			this->check_cgi();
			sleep(1);
		}
		std::cout << "CGI exited with status " << this->check_cgi() <<std::endl;
		read(this->pipe_receive_cgi_answer[0], buffer, 1000);
		if (std::remove(this->tmp_file_name.c_str()) < 0)
			throw(CgiControllerSystemFunctionFailed("remove"));
		std::cout << buffer;
		// <<
		// this is only for testing purposes 
	}
	else
		throw(CgiControllerSystemFunctionFailed("fork"));
}

e_cgi_status Cgi_Controller::check_cgi()
{
	int		status;
	pid_t	result;

	if (this->status != CGI_RUNNING)
		return (this->status);
	if (this->check_cgi_executor_timeout())
	{
		std::cout << "Child process timed out, getting killed..." << std::endl;
		if (kill(this->executor_pid_id, SIGKILL) < 0)
			throw(CgiControllerSystemFunctionFailed("kill"));
		this->status = CGI_KILLED_TIMEOUT;
		return (this->status);
	}
	result = waitpid(this->executor_pid_id, &status, WNOHANG);
	if (result == 0)
	{
		std::cout << "Child process is still running..." << std::endl;
		return (this->status);
	}
	else if (result == this->executor_pid_id)
	{
		std::cout << "Child process has finished!" << std::endl;
		if (WIFEXITED(status))
		{
			this->status = CGI_EXITED_NORMAL;
			std::cout << "Child exited with status: " << WEXITSTATUS(status) << std::endl;
		}
		else
		{
			this->status = CGI_EXITED_ERROR;
			// throw(CgiControllerSystemFunctionFailed("child process exit"));
		}
	}
	else
		throw(CgiControllerSystemFunctionFailed("waitpid"));
	return (this->status);
}

bool Cgi_Controller::check_cgi_executor_timeout()
{
	std::cout << "Checking for CGI Timeout..." << std::endl;
	if (time(NULL) - this->executor_start_time > CGI_TIMEOUT_SEC)
		return (true);
	return (false);
}

Cgi_Controller::CgiControllerSystemFunctionFailed::CgiControllerSystemFunctionFailed(std::string function_name) : _function_name(function_name)
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

std::string Cgi_Controller::get_random_string(size_t length)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const size_t charset_size = sizeof(charset) - 1;
	std::string random_string;

	std::srand(static_cast<unsigned int>(std::time(0)));

	for (size_t i = 0; i < length; ++i)
	{
		random_string += charset[std::rand() % charset_size];
	}

	return (random_string);
}