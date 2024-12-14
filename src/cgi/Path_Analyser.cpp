/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path_Analyser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Path_Analyser.hpp"

Path_Analyser::Path_Analyser() : script_name(""), script_path(""),
	path_translated(""), script_extension(""), path_info(""), query_string("")
{
	return ;
}

Path_Analyser::Path_Analyser(const Path_Analyser &other)
{
	this->script_name = other.script_name;
	this->script_path = other.script_path;
	this->path_translated = other.path_translated;
	this->script_extension = other.script_extension;
	this->path_info = other.path_info;
	this->query_string = other.query_string;
	return ;
}

Path_Analyser &Path_Analyser::operator=(const Path_Analyser &other)
{
	if (this != &other)
	{
		this->script_name = other.script_name;
		this->script_path = other.script_path;
		this->path_translated = other.path_translated;
		this->script_extension = other.script_extension;
		this->path_info = other.path_info;
		this->query_string = other.query_string;
	}
	return (*this);
}

Path_Analyser::~Path_Analyser(void)
{
	return ;
}

void Path_Analyser::analyse(std::string path, std::string root)
{

	this->path_to_process = path;
	this->original_path_to_process = path;
	this->extract_query_string();
	this->find_first_dot();
	this->find_first_slash_after_dot();
	this->script_extension = this->path_to_process.substr(this->_pos_dot + 1, this->_pos_slash
			- this->_pos_dot - 1);
	this->script_path = this->path_to_process.substr(0, this->_pos_slash);
	this->path_translated = root + this->script_path;
    util::replace_all(this->path_translated, "//", "/");
	this->path_info = this->path_to_process.substr(this->_pos_slash);
	this->_pos_script_start = this->script_path.rfind("/");
	this->script_name = this->script_path.substr(this->_pos_script_start + 1);
}

void Path_Analyser::find_first_slash_after_dot()
{
	this->_pos_slash = this->path_to_process.find("/", this->_pos_dot);
	if (this->_pos_slash == std::string::npos)
		this->_pos_slash = this->path_to_process.length();
}

void Path_Analyser::find_first_dot()
{
	this->_pos_dot = this->path_to_process.find(".");
	if (this->_pos_dot == std::string::npos)
	{
		if (this->path_to_process[this->path_to_process.length() - 1] != '/')
			this->path_to_process += "/";
		this->path_to_process += "time.py";
		this->_pos_dot = this->path_to_process.find(".");
	}
}

void Path_Analyser::extract_query_string()
{
	size_t	query_pos;

	query_pos = this->path_to_process.find('?');
	if (query_pos != std::string::npos)
	{
		this->query_string = this->path_to_process.substr(query_pos + 1);
		this->path_to_process = this->path_to_process.substr(0, query_pos);
	}
	else
		this->query_string = "";
}

std::ostream &operator<<(std::ostream &os, Path_Analyser const &path_analyser)
{
	std::cout << "Result of Path analysis__________________" << std::endl << std::endl;
	std::cout << "original path to process: " << path_analyser.original_path_to_process << std::endl;
	std::cout << "script_name : " << path_analyser.script_name << std::endl;
	std::cout << "script_path : " << path_analyser.script_path << std::endl;
	std::cout << "path_translated : " << path_analyser.path_translated << std::endl;
	std::cout << "script_extension : " << path_analyser.script_extension << std::endl;
	std::cout << "path_info : " << path_analyser.path_info << std::endl;
	std::cout << "query_string : " << path_analyser.query_string << std::endl;
	std::cout << std::endl << "___________________________________";
	std::cout << std::endl << std::endl << std::endl;
	return (os);
}