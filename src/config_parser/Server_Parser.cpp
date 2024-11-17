/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Parser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/15 16:39:08 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Parser.hpp"
#include "webserv.hpp"

Server_Parser::Server_Parser(std::string server_str) : _server_str(server_str)
{
	this->assign_handlers();
	return ;
}

Server_Parser::Server_Parser(const Server_Parser &other)
{
	this->assign_handlers();
	this->_server_str = other._server_str;
	this->_keywords_str = other._keywords_str;
	this->_location_parser = other._location_parser;
	this->_root_handler = other._root_handler;
	this->_index_handler = other._index_handler;
	this->_error_page_handler = other._error_page_handler;
	this->_client_max_body_size_handler = other._client_max_body_size_handler;
	this->_autoindex_handler = other._autoindex_handler;
	this->_listen_handler = other._listen_handler;
	this->_server_name_handler = other._server_name_handler;
	this->_return_handler = other._return_handler;
	return ;
}

Server_Parser &Server_Parser::operator=(const Server_Parser &other)
{
	if (this != &other)
	{
		this->_server_str = other._server_str;
		this->_keywords_str = other._keywords_str;
		this->_location_parser = other._location_parser;
		this->_root_handler = other._root_handler;
		this->_index_handler = other._index_handler;
		this->_error_page_handler = other._error_page_handler;
		this->_client_max_body_size_handler = other._client_max_body_size_handler;
		this->_autoindex_handler = other._autoindex_handler;
		this->_listen_handler = other._listen_handler;
		this->_server_name_handler = other._server_name_handler;
	this->_return_handler = other._return_handler;
	}
	return (*this);
}

Server_Parser::~Server_Parser(void)
{
	return ;
}

void Server_Parser::assign_handlers()
{
	this->keyword_handlers["root"] = &Server_Parser::handle_root;
	this->keyword_handlers["index"] = &Server_Parser::handle_index;
	this->keyword_handlers["error_page"] = &Server_Parser::handle_error_page;
	this->keyword_handlers["client_max_body_size"] = &Server_Parser::handle_client_max_body_size;
	this->keyword_handlers["autoindex"] = &Server_Parser::handle_autoindex;
	this->keyword_handlers["listen"] = &Server_Parser::handle_listen;
	this->keyword_handlers["server_name"] = &Server_Parser::handle_server_name;
	this->keyword_handlers["return"] = &Server_Parser::handle_return;
}

void Server_Parser::parse_server_block(void)
{
	this->handle_location_strings();
	this->handle_server_directive_str();
}

void Server_Parser::handle_location_strings(void)
{
	std::string remaining_server_block = this->_server_str;
	this->_server_str = "";
	while (remaining_server_block.length() > 0)
	{
		if (this->extract_location_block(remaining_server_block) < 0)
			break ;
	}
}

int Server_Parser::extract_location_block(std::string &remaining_server_block)
{
	int	index_start_location_block;
	int	index_opening_bracket_location_block;
	int	index_closing_bracket_location_block;

	index_start_location_block = remaining_server_block.find("location");
	if (index_start_location_block < 0)
	{
		this->_keywords_str += remaining_server_block;
		return (-1);
	}
	std::string location = this->get_location(remaining_server_block,
			index_start_location_block, index_opening_bracket_location_block);
	index_closing_bracket_location_block = util::find_matching_closing_bracket_no_nesting(remaining_server_block,
			index_opening_bracket_location_block);
	this->check_for_duplicate_location(location);
	if (index_closing_bracket_location_block < 0)
		throw ClosingBracketForLocationBlockNotFound();
	this->parse_location_string(location, remaining_server_block,
		index_opening_bracket_location_block,
		index_closing_bracket_location_block);
	this->_keywords_str += remaining_server_block.substr(0,
			index_start_location_block);
	remaining_server_block = remaining_server_block.substr(index_closing_bracket_location_block
			+ 1);
	return (1);
}

void Server_Parser::check_for_duplicate_location(std::string new_location)
{
	for (std::vector<Location_Parser>::iterator it = this->_location_parser.begin(); it != this->_location_parser.end(); it++)
	{
		if(new_location == it->get_location())
			throw DuplicateLocationDefinitionWithinOneServerBlock(new_location);
	}
}

void Server_Parser::parse_location_string(std::string location,
	std::string remaining_server_block,
	int index_opening_bracket_location_block,
	int index_closing_bracket_location_block)
{
	Location_Parser	location_parser;

	std::string location_block_str;
	location_parser.set_location(location);
	location_block_str = remaining_server_block.substr(index_opening_bracket_location_block
			+ 1, index_closing_bracket_location_block
			- index_opening_bracket_location_block - 1);
	location_parser.set_location_block_str(location_block_str);
	location_parser.parse_location_block();
	this->_location_parser.push_back(location_parser);
}

std::string Server_Parser::get_location(std::string remaining_server_block,
	int index_start_location_block, int &index_opening_bracket_location_block)
{
	std::string location = "";
	std::vector<std::string> location_split = util::split(remaining_server_block,
			' ', 3, index_start_location_block);
	if (location_split[1][0] == '{' || location_split[1][0] == '}')
		throw NoLocationDefinedInsideLocationBlock();
	this->get_location_string(location_split[1], location);
	index_opening_bracket_location_block = index_start_location_block
		+ std::string("location").length() + 1 + location.length();
	if (location.length() == location_split[1].length())
	{
		if (location_split[2][0] != '{')
			throw NoOpeningBracketInsideLocationBlock();
		index_opening_bracket_location_block += 1;
		return (location);
	}
	if (location_split[1][location.length()] != '{')
		throw NoOpeningBracketInsideLocationBlock();
	return (location);
}

void Server_Parser::get_location_string(std::string src_str,
	std::string &res_location)
{
	for (std::string::iterator it = src_str.begin(); it != src_str.end(); ++it)
	{
		if (*it == '{' || *it == '}')
			break ;
		res_location += *it;
	}
}

void Server_Parser::handle_server_directive_str(void)
{
	std::vector<std::string> key_value_strings;
	std::vector<std::string> key_value_vector;
	key_value_strings = util::split(this->_keywords_str, ';');
	std::vector<std::string>::iterator it_key_val;
	for (it_key_val = key_value_strings.begin(); it_key_val != key_value_strings.end(); it_key_val++)
	{
		key_value_vector = util::split(*it_key_val, ' ');
		std::map<std::string,
			void (Server_Parser::*)(std::vector<std::string> &)>::iterator it = this->keyword_handlers.find(key_value_vector[0]);
		if (it != this->keyword_handlers.end())
		{
			(this->*(it->second))(key_value_vector);
		}
		else
			throw UnknownKeywordInServerBlock(key_value_vector[0]);
	}
}

void Server_Parser::handle_index(std::vector<std::string> &key_val)
{
	this->_index_handler.check_and_add(key_val);
}
void Server_Parser::handle_error_page(std::vector<std::string> &key_val)
{
	this->_error_page_handler.check_and_add(key_val);
}

void Server_Parser::handle_client_max_body_size(std::vector<std::string> &key_val)
{
	this->_client_max_body_size_handler.check_and_add(key_val);
}

void Server_Parser::handle_autoindex(std::vector<std::string> &key_val)
{
	this->_autoindex_handler.check_and_add(key_val);
}

void Server_Parser::handle_root(std::vector<std::string> &key_val)
{
	std::vector<std::string> dummy;
	this->_root_handler.check_and_add(key_val, dummy);
}

void Server_Parser::handle_listen(std::vector<std::string> &key_val)
{
	this->_listen_handler.check_and_add(key_val);
}

void Server_Parser::handle_server_name(std::vector<std::string> &key_val)
{
	this->_server_name_handler.check_and_add(key_val);
}

void Server_Parser::handle_return(std::vector<std::string> &key_val)
{
	this->_return_handler.check_and_add(key_val);
}

Directive_Root &Server_Parser::get_root_handler()
{
	return (this->_root_handler);
}
std::vector<Location_Parser> &Server_Parser::get_location_parser()
{
	return (this->_location_parser);
}
Directive_Index &Server_Parser::get_index_handler()
{
	return (this->_index_handler);
}
Directive_Error_Page &Server_Parser::get_error_page_handler()
{
	return (this->_error_page_handler);
}
Directive_Client_Max_Body_Size &Server_Parser::get_client_max_body_size_handler()
{
	return (this->_client_max_body_size_handler);
}
Directive_Autoindex &Server_Parser::get_autoindex_handler()
{
	return (this->_autoindex_handler);
}
Directive_Listen &Server_Parser::get_listen_handler()
{
	return (this->_listen_handler);
}
Directive_Server_Name &Server_Parser::get_server_name_handler()
{
	return (this->_server_name_handler);
}

Directive_Return &Server_Parser::get_return_handler()
{
	return (this->_return_handler);
}

Server_Parser::UnknownKeywordInServerBlock::UnknownKeywordInServerBlock(std::string keyword_name) : _keyword_name(keyword_name)
{
}

Server_Parser::UnknownKeywordInServerBlock::~UnknownKeywordInServerBlock() throw()
{
}

const char *Server_Parser::UnknownKeywordInServerBlock::what() const throw()
{
	this->_msg = "keyword: ";
	this->_msg += this->_keyword_name;
	this->_msg += "\n";
	this->_msg
		+= "There was an unknown keyword encountered inside a server block!\n";
	return (this->_msg.c_str());
}


Server_Parser::DuplicateLocationDefinitionWithinOneServerBlock::DuplicateLocationDefinitionWithinOneServerBlock(std::string location_name) : _location_name(location_name)
{
}

Server_Parser::DuplicateLocationDefinitionWithinOneServerBlock::~DuplicateLocationDefinitionWithinOneServerBlock() throw()
{
}

const char *Server_Parser::DuplicateLocationDefinitionWithinOneServerBlock::what() const throw()
{
	this->_msg = "location name: ";
	this->_msg += this->_location_name;
	this->_msg += "\n";
	this->_msg
		+= "A location was defined multiple times inside one server block!";
	return (this->_msg.c_str());
}

const char *Server_Parser::ClosingBracketForLocationBlockNotFound::what() const throw()
{
	return ("The closing bracket for a location block couldn't be found or unsupported nesting inside the location block identified!\n");
}

const char *Server_Parser::NoOpeningBracketInsideLocationBlock::what() const throw()
{
	return ("Opening Bracket was not found after the first argument within location block!\n");
}

const char *Server_Parser::NoLocationDefinedInsideLocationBlock::what() const throw()
{
	return ("No location was given inside a location block opening bracket cam directly after location keyword!\n");
}
