/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi_Executor.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/30 13:50:39 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi_Controller.hpp"
#include "Cgi_Executor.hpp"

Cgi_Executor::Cgi_Executor(Cgi_Controller *val_corresponding_controller)
	: corresponding_controller(val_corresponding_controller), env_arr(NULL),
		argv_arr(NULL)
{
	return ;
}

Cgi_Executor::Cgi_Executor(const Cgi_Executor &other)
{
	// this->pipe_write_request_body_to_cgi[0] = other.pipe_write_request_body_to_cgi[0];
	// this->pipe_write_request_body_to_cgi[1] = other.pipe_write_request_body_to_cgi[1];
	this->corresponding_controller = other.corresponding_controller;
	return ;
}

Cgi_Executor &Cgi_Executor::operator=(const Cgi_Executor &other)
{
	if (this != &other)
	{
		// this->pipe_write_request_body_to_cgi[0] = other.pipe_write_request_body_to_cgi[0];
		// this->pipe_write_request_body_to_cgi[1] = other.pipe_write_request_body_to_cgi[1];
		this->corresponding_controller = other.corresponding_controller;
	}
	return (*this);
}

Cgi_Executor::~Cgi_Executor(void)
{
	return ;
}

void Cgi_Executor::start_cgi()
{
	this->body = "Hello, I am the hardcoded body \n\n\n";
	this->init_env_map();
	this->env_map_to_env_arr();
	this->create_argv_arr();
	this->put_request_body_into_stdin();
	if (dup2(this->corresponding_controller->pipe_receive_cgi_answer[1],
				STDOUT_FILENO) < 0)
		throw(CgiExecutorSystemFunctionFailed("dup2"));
	if (close(this->corresponding_controller->pipe_receive_cgi_answer[1]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	this->run_script();
}

void Cgi_Executor::put_request_body_into_stdin()
{
	const char	*temp_file;
	int			fd;

	temp_file = this->corresponding_controller->tmp_file_name.c_str();
	std::ofstream out(temp_file);
	out << this->body;
	out.close();
	fd = open(temp_file, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		_exit(1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		_exit(1);
	}
	if (close(fd) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
}

void Cgi_Executor::init_env_map()
{
	std::ostringstream oss;
	oss << this->body.size();
	std::string content_length = oss.str();
	std::cout << "content length is <" << content_length;
	std::cout << std::endl
				<< std::endl
				<< std::flush;
	this->env_map["CONTENT_LENGTH"] = content_length;
	this->env_map["CONTENT_TYPE"] = "VAL_CONTENT_TYPE";
	this->env_map["GATEWAY_INTERFACE"] = "VAL_GATEWAY_INTERFACE";
	this->env_map["PATH_INFO"] = "VAL_PATH_INFO";
	this->env_map["PATH_TRANSLATED"] = "VAL_PATH_TRANSLATED";
	this->env_map["QUERY_STRING"] = "VAL_QUERY_STRING";
	this->env_map["REMOTE_ADDR"] = "VAL_REMOTE_ADDR";
	this->env_map["REMOTE_HOST"] = "VAL_REMOTE_HOST";
	this->env_map["REQUEST_METHOD"] = "VAL_REQUEST_METHOD";
	this->env_map["SCRIPT_NAME"] = "VAL_SCRIPT_NAME";
	this->env_map["SERVER_NAME"] = "VAL_SERVER_NAME";
	this->env_map["SERVER_PORT"] = "VAL_SERVER_PORT";
	this->env_map["SERVER_PROTOCOL"] = "VAL_SERVER_PROTOCOL";
	this->env_map["SERVER_SOFTWARE"] = "VAL_SERVER_SOFTWARE";
	// add selected http headers to env map
}

void Cgi_Executor::env_map_to_env_arr()
{
	int	i;

	this->env_arr = new char *[this->env_map.size()];
	if (!this->env_arr)
		throw(CgiExecutorSystemFunctionFailed("new"));
	i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = this->env_map.begin(); it != this->env_map.end(); ++it)
	{
		std::string val = it->first + "=" + it->second;
		this->env_arr[i] = new char[val.size() + 1];
		if (!this->env_arr[i])
			throw(CgiExecutorSystemFunctionFailed("new"));
		std::strcpy(this->env_arr[i], val.c_str());
		i++;
		this->env_arr[i] = NULL;
	}
}

void Cgi_Executor::create_argv_arr()
{
	std::string arg_0 = "/usr/bin/python3";
	std::string arg_1 = "./print_env_body.py";
	this->argv_arr = new char *[3];
	if (!this->argv_arr)
		throw(CgiExecutorSystemFunctionFailed("new"));
	this->argv_arr[0] = new char[arg_0.size() + 1];
	if (!this->argv_arr[0])
		throw(CgiExecutorSystemFunctionFailed("new"));
	std::strcpy(this->argv_arr[0], arg_0.c_str());
	this->argv_arr[1] = new char[arg_1.size() + 1];
	if (!this->argv_arr[1])
		throw(CgiExecutorSystemFunctionFailed("new"));
	std::strcpy(this->argv_arr[1], arg_1.c_str());
	this->argv_arr[2] = NULL;
}

void Cgi_Executor::run_script()
{
	execve(this->argv_arr[0], this->argv_arr, this->env_arr);
	throw(CgiExecutorSystemFunctionFailed("execve"));
}

Cgi_Executor::CgiExecutorSystemFunctionFailed::CgiExecutorSystemFunctionFailed(std::string function_name)
	: _function_name(function_name)
{
}

Cgi_Executor::CgiExecutorSystemFunctionFailed::~CgiExecutorSystemFunctionFailed() throw()
{
}

const char *Cgi_Executor::CgiExecutorSystemFunctionFailed::what() const throw()
{
	this->_msg = "system call: ";
	this->_msg += this->_function_name;
	this->_msg = " failed inside the Cgi_Executor";
	this->_msg += "\n";
	return (this->_msg.c_str());
}