/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive_Autoindex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 17:07:07 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/08 17:34:23 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive_Autoindex.hpp"
#include "webserv.hpp"

Directive_Autoindex::Directive_Autoindex()
{
	this->_directive_name = "autoindex";
	this->_duplicate_definition_allowed = false;
	this->_many_args_allowed = false;
	this->_allowed_values.push_back("on");
	this->_allowed_values.push_back("off");
	return ;
}

Directive_Autoindex::Directive_Autoindex(const Directive_Autoindex &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Autoindex &Directive_Autoindex::operator=(const Directive_Autoindex &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Autoindex::~Directive_Autoindex(void)
{
	return ;
}

void Directive_Autoindex::check_and_add(std::vector<std::string> key_val)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_allowed_values();
	this->check_for_duplicate_identifier(this->_config_defs);
	if (key_val[1] == "on")
		this->_config_defs.push_back(true);
	else if (key_val[1] == "off")
		this->_config_defs.push_back(false);
	else
		throw UnallowedArgumentForDirective(this->_directive_name);
}

std::vector<bool> &Directive_Autoindex::get_config_defs(void)
{
	return (this->_config_defs);
}
