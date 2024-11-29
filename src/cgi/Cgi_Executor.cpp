/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi_Executor.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/29 18:13:47 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi_Controller.hpp"
#include "Cgi_Executor.hpp"

Cgi_Executor::Cgi_Executor(Cgi_Controller *val_corresponding_controller)
	: corresponding_controller(val_corresponding_controller)
{
	return ;
}

Cgi_Executor::Cgi_Executor(const Cgi_Executor &other)
{
	this->pipe_write_request_body_to_cgi[0] = other.pipe_write_request_body_to_cgi[0];
	this->pipe_write_request_body_to_cgi[1] = other.pipe_write_request_body_to_cgi[1];
	this->corresponding_controller = other.corresponding_controller;
	return ;
}

Cgi_Executor &Cgi_Executor::operator=(const Cgi_Executor &other)
{
	if (this != &other)
	{
		this->pipe_write_request_body_to_cgi[0] = other.pipe_write_request_body_to_cgi[0];
		this->pipe_write_request_body_to_cgi[1] = other.pipe_write_request_body_to_cgi[1];
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
	if (pipe(this->pipe_write_request_body_to_cgi) < 0)
		throw(CgiExecutorSystemFunctionFailed("pipe"));
	if (dup2(this->pipe_write_request_body_to_cgi[0], STDIN_FILENO) < 0)
		throw(CgiExecutorSystemFunctionFailed("dup2"));
	if (dup2(this->corresponding_controller->pipe_receive_cgi_answer[1],
				STDOUT_FILENO) < 0)
		throw(CgiExecutorSystemFunctionFailed("dup2"));
	if (close(this->corresponding_controller->pipe_receive_cgi_answer[0]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	if (close(this->corresponding_controller->pipe_receive_cgi_answer[1]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	if (close(this->pipe_write_request_body_to_cgi[0]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	this->write_request_body_to_pipe();
	if (close(this->pipe_write_request_body_to_cgi[1]) < 0)
		throw(CgiExecutorSystemFunctionFailed("close"));
	this->init_env_map();
	this->run_script();
}

void Cgi_Executor::write_request_body_to_pipe()
{
	int	bytes_written;

	this->body = "Hello, I am the hardcoded body \n\n\n";
	if (this->body.size() > 0)
	{
		bytes_written = write(this->pipe_write_request_body_to_cgi[1],
								this->body.c_str(),
								this->body.size());
		if (bytes_written < 0)
			throw(CgiExecutorSystemFunctionFailed("write"));
		if (static_cast<size_t>(bytes_written) < this->body.size())
			throw(CgiExecutorSystemFunctionFailed("written less bytes than supposed to!"));
	}
}

void Cgi_Executor::init_env_map()
{
	std::ostringstream oss;
	oss << this->body;
	std::string content_length = oss.str();
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
}

void Cgi_Executor::env_map_to_env_arr()
{
	std::vector<std::string> env_strings;
	for (std::map<std::string,
					std::string>::const_iterator it = this->env_map.begin();
			it != this->env_map.end();
			++it)
	{
		env_strings.push_back(it->first + "=" + it->second);
	}
	std::vector<char *> envp;
	for (std::vector<std::string>::iterator it = env_strings.begin(); it != env_strings.end(); ++it)
	{
		this->env_arr.push_back(const_cast<char *>(it->c_str()));
	}
	this->env_arr.push_back(NULL);
}

void Cgi_Executor::run_script()
{
	const char	*argv[3];

	argv[0] = "/usr/bin/python3";
	argv[1] = "./print_env_body.py";
	argv[0] = NULL;
	char *const *envp = &this->env_arr[0];
	execve(argv[0], argv, envp);
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
	this->_msg += this->function_name;
	this->_msg = " failed inside the Cgi_Executor";
	this->_msg += "\n";
	return (this->_msg.c_str());
}