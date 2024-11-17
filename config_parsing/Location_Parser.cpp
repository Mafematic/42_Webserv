/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_Parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:29:15 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationLocation_Parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 13:47:54 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/02 13:56:13 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location_Parser.hpp"
#include "external.hpp"

Location_Parser::Location_Parser() : _location_block_str(""), _location("")
{
	this->assign_handlers();
	return ;
}

Location_Parser::Location_Parser(std::string location_block_str) : _location_block_str(location_block_str),
	_location("")
{
	this->assign_handlers();
	return ;
}

Location_Parser::Location_Parser(const Location_Parser &other)
{
	this->_root_handler = other._root_handler;
	this->_alias_handler = other._alias_handler;
	this->_index_handler = other._index_handler;
	this->_error_page_handler = other._error_page_handler;
	this->_client_max_body_size_handler = other._client_max_body_size_handler;
	this->_allowed_methods_handler = other._allowed_methods_handler;
	this->_autoindex_handler = other._autoindex_handler;
	this->_location_block_str = other._location_block_str;
	this->_location = other._location;
	this->_return_handler = other._return_handler;
	this->assign_handlers();
	return ;
}

Location_Parser &Location_Parser::operator=(const Location_Parser &other)
{
	if (this != &other)
	{
		this->_root_handler = other._root_handler;
		this->_alias_handler = other._alias_handler;
		this->_index_handler = other._index_handler;
		this->_error_page_handler = other._error_page_handler;
		this->_client_max_body_size_handler = other._client_max_body_size_handler;
		this->_allowed_methods_handler = other._allowed_methods_handler;
		this->_autoindex_handler = other._autoindex_handler;
		this->_location_block_str = other._location_block_str;
		this->_location = other._location;
		this->_return_handler = other._return_handler;
		this->assign_handlers();
	}
	return (*this);
}

Location_Parser::~Location_Parser(void)
{
	return ;
}

void Location_Parser::assign_handlers()
{
	this->keyword_handlers["root"] = &Location_Parser::handle_root;
	this->keyword_handlers["alias"] = &Location_Parser::handle_alias;
	this->keyword_handlers["index"] = &Location_Parser::handle_index;
	this->keyword_handlers["error_page"] = &Location_Parser::handle_error_page;
	this->keyword_handlers["client_max_body_size"] = &Location_Parser::handle_client_max_body_size;
	this->keyword_handlers["allowed_methods"] = &Location_Parser::handle_allowed_methods;
	this->keyword_handlers["autoindex"] = &Location_Parser::handle_autoindex;
	this->keyword_handlers["return"] = &Location_Parser::handle_return ;
}

void Location_Parser::set_location_block_str(std::string location_block_str)
{
	this->_location_block_str = location_block_str;
}

void Location_Parser::set_location(std::string location)
{
	this->_location = location;
}

void Location_Parser::parse_location_block()
{
	std::vector<std::string> key_value_strings;
	std::vector<std::string> key_value_vector;
	if (this->_location_block_str.empty())
		throw EmptyLocationDefinition();
	key_value_strings = util::split(this->_location_block_str, ';');
	std::vector<std::string>::iterator it_key_val;
	for (it_key_val = key_value_strings.begin(); it_key_val != key_value_strings.end(); it_key_val++)
	{
		key_value_vector = util::split(*it_key_val, ' ');
		std::map<std::string,
			void (Location_Parser::*)(std::vector<std::string> &)>::iterator it = this->keyword_handlers.find(key_value_vector[0]);
		if (it != this->keyword_handlers.end())
		{
			(this->*(it->second))(key_value_vector);
		}
		else
			throw UnknownKeywordInLocationBlock(key_value_vector[0]);
	}
}

void Location_Parser::handle_index(std::vector<std::string> &key_val)
{
	this->_index_handler.check_and_add(key_val);
}
void Location_Parser::handle_error_page(std::vector<std::string> &key_val)
{
	this->_error_page_handler.check_and_add(key_val);
}

void Location_Parser::handle_client_max_body_size(std::vector<std::string> &key_val)
{
	this->_client_max_body_size_handler.check_and_add(key_val);
}

void Location_Parser::handle_allowed_methods(std::vector<std::string> &key_val)
{
	this->_allowed_methods_handler.check_and_add(key_val);
}
void Location_Parser::handle_autoindex(std::vector<std::string> &key_val)
{
	this->_autoindex_handler.check_and_add(key_val);
}

void Location_Parser::handle_root(std::vector<std::string> &key_val)
{
	this->_root_handler.check_and_add(key_val,
		this->_alias_handler.get_config_defs());
}

void Location_Parser::handle_alias(std::vector<std::string> &key_val)
{
	this->_alias_handler.check_and_add(key_val,
		this->_root_handler.get_config_defs());
}

void Location_Parser::handle_return(std::vector<std::string> &key_val)
{
	this->_return_handler.check_and_add(key_val);
}

std::string Location_Parser::get_location(void)
{
	return (this->_location);
}

Directive_Root &Location_Parser::get_root_handler()
{
	return (this->_root_handler);
}

Directive_Alias &Location_Parser::get_alias_handler()
{
	return (this->_alias_handler);
}

Directive_Index &Location_Parser::get_index_handler()
{
	return (this->_index_handler);
}

Directive_Error_Page &Location_Parser::get_error_page_handler()
{
	return (this->_error_page_handler);
}

Directive_Client_Max_Body_Size &Location_Parser::get_client_max_body_size_handler()
{
	return (this->_client_max_body_size_handler);
}

Directive_Autoindex &Location_Parser::get_autoindex_handler()
{
	return (this->_autoindex_handler);
}

Directive_Allowed_Methods &Location_Parser::get_allowed_methods_handler()
{
	return (this->_allowed_methods_handler);
}

Directive_Return &Location_Parser::get_return_handler()
{
	return (this->_return_handler);
}

const char *Location_Parser::EmptyLocationDefinition::what() const throw()
{
	return ("The location block does not contain any directives!\n");
}

Location_Parser::UnknownKeywordInLocationBlock::UnknownKeywordInLocationBlock(std::string keyword_name) : _keyword_name(keyword_name)
{
}

Location_Parser::UnknownKeywordInLocationBlock::~UnknownKeywordInLocationBlock() throw()
{
}

const char *Location_Parser::UnknownKeywordInLocationBlock::what() const throw()
{
	this->_msg = "keyword: ";
	this->_msg += this->_keyword_name;
	this->_msg += "\n";
	this->_msg
		+= "There was an unknown keyword encountered inside a location block!";
	return (this->_msg.c_str());
}

void Location_Parser::print(void)
{
	std::cout << std::endl;
	std::cout << "**********************";
	std::cout << std::endl;
	std::cout << "Location_Parser" << std::endl;
	std::cout << "Location >>" << this->_location << "<<";
	std::cout << std::endl;
	std::cout << "Block String >>" << this->_location_block_str << "<<";
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "**********************";
	std::cout << std::endl;
}
