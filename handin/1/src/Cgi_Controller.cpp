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

Cgi_Controller::Cgi_Controller() : tmp_file_name(""), status(CGI_RUNNING),
	tmp_file_was_deleted(false)
{
}

Cgi_Controller::Cgi_Controller(Client client) : tmp_file_name(""),
	corresponding_client(client), status(CGI_RUNNING),
	tmp_file_was_deleted(false)
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
	this->corresponding_client = other.corresponding_client;
	this->tmp_file_was_deleted = other.tmp_file_was_deleted;
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
		this->corresponding_client = other.corresponding_client;
		this->tmp_file_was_deleted = other.tmp_file_was_deleted;
	}
	return (*this);
}

Cgi_Controller::~Cgi_Controller(void)
{
	return ;
}

void Cgi_Controller::start_cgi()
{
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
		if (close(this->pipe_receive_cgi_answer[1]) < 0)
			throw(CgiControllerSystemFunctionFailed("close"));
	}
	else if (this->executor_pid_id < 0)
		throw(CgiControllerSystemFunctionFailed("fork"));
}

int	Cgi_Controller::kill_child()
{
	return kill(this->executor_pid_id, SIGKILL);

}

e_cgi_status Cgi_Controller::check_cgi()
{
	int		status;
	pid_t	result;

	if (this->status != CGI_RUNNING)
		return (this->status);
	if (this->check_cgi_executor_timeout())
	{
		// std::cout << "Child process timed out,
			// getting killed..." << std::endl;
		if (kill_child() < 0)
			throw(CgiControllerSystemFunctionFailed("kill"));
		this->status = CGI_KILLED_TIMEOUT;
		this->remove_cgi_tmp_infile();
		return (this->status);
	}
	result = waitpid(this->executor_pid_id, &status, WNOHANG);
	if (result == 0)
	{
		// std::cout << "Child process is still running..." << std::endl;
		return (this->status);
	}
	else if (result == this->executor_pid_id)
	{
		// std::cout << "Child process has finished!" << std::endl;
		this->remove_cgi_tmp_infile();
		if (WIFEXITED(status))
		{
			if (WEXITSTATUS(status) == 0)
				this->status = CGI_EXITED_NORMAL;
			else
				this->status = CGI_EXITED_ERROR;
			// std::cout << "Child exited with status: " << WEXITSTATUS(status) << std::endl;
		}
		else
			this->status = CGI_EXITED_ERROR;
	}
	else
		throw(CgiControllerSystemFunctionFailed("waitpid"));
	return (this->status);
}

bool Cgi_Controller::check_cgi_executor_timeout()
{
	// std::cout << "Checking for CGI Timeout..." << std::endl;
	if (time(NULL) - this->executor_start_time > CGI_TIMEOUT_SEC)
		return (true);
	return (false);
}

void Cgi_Controller::remove_cgi_tmp_infile()
{
	if (!this->tmp_file_was_deleted)
	{
		if (std::remove(this->tmp_file_name.c_str()) != 0)
			throw(CgiControllerSystemFunctionFailed("remove"));
		this->tmp_file_was_deleted = true;
	}
	// std::cout << "CGI CONTROLLER: TMP FILE WAS DELETED!" << std::endl;
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
	const char		charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const size_t	charset_size = sizeof(charset) - 1;

	std::string random_string;
	std::srand(static_cast<unsigned int>(std::time(0)));
	for (size_t i = 0; i < length; ++i)
	{
		random_string += charset[std::rand() % charset_size];
	}
	return (random_string);
}
