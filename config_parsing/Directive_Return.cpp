/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive_Return.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 17:07:07 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/08 17:34:23 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive_Return.hpp"
#include "external.hpp"

Directive_Return::Directive_Return()
{
	this->_directive_name = "return";
	this->_duplicate_definition_allowed = false;
	this->_many_args_allowed = true;
	return ;
}

Directive_Return::Directive_Return(const Directive_Return &other) : Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Return &Directive_Return::operator=(const Directive_Return &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Return::~Directive_Return(void)
{
	return ;
}

void Directive_Return::check_and_add(std::vector<std::string> key_val)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_allowed_values();
	this->check_for_duplicate_identifier(this->_config_defs);
	if (key_val.size() != 3)
		throw InvalidNumberOfArguments(this->_directive_name);
	this->process_return_definition(this->_key_val);
}

std::vector<util::Return_Definition> &Directive_Return::get_config_defs(void)
{
	return (this->_config_defs);
}

void Directive_Return::process_return_definition(std::vector<std::string> &key_val)
{
	int	num;

	util::Return_Definition ret_def;
	num = util::is_valid_http_code(key_val[1]);
	if (num == -1)
		throw InvalidReturnCodeFormat(this->get_directive_name());
	ret_def.status_code = num;
	ret_def.url = key_val[2];
	this->_config_defs.push_back(ret_def);
}

Directive_Return::InvalidReturnCodeFormat::InvalidReturnCodeFormat(std::string directive_name) : _directive_name(directive_name)
{
}

Directive_Return::InvalidReturnCodeFormat::~InvalidReturnCodeFormat() throw()
{
}

const char *Directive_Return::InvalidReturnCodeFormat::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg
		+= "The provided return code must be only numbers between 300 and 599!";
	return (this->_msg.c_str());
}