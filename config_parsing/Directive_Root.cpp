/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive_Root.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 17:07:07 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/08 17:34:23 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive_Root.hpp"
#include "external.hpp"

Directive_Root::Directive_Root()
{
	this->_directive_name = "root";
	this->_duplicate_definition_allowed = false;
	this->_many_args_allowed = false;
	return ;
}

Directive_Root::Directive_Root(const Directive_Root &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Root &Directive_Root::operator=(const Directive_Root &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Root::~Directive_Root(void)
{
	return ;
}

void Directive_Root::check_and_add(std::vector<std::string> key_val,
									std::vector<std::string> &alias)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_allowed_values();
	this->check_for_duplicate_identifier(this->_config_defs);
	if (alias.size() > 0)
		throw AliasNotAllowedWithRoot(this->get_directive_name());
	this->_config_defs.push_back(this->_key_val[1]);
}

std::vector<std::string> &Directive_Root::get_config_defs(void)
{
	return (this->_config_defs);
}