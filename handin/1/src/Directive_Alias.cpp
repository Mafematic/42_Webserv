#include "Directive_Alias.hpp"
#include "webserv.hpp"

Directive_Alias::Directive_Alias()
{
	this->_directive_name = "alias";
	this->_duplicate_definition_allowed = false;
	this->_many_args_allowed = false;
	return ;
}

Directive_Alias::Directive_Alias(const Directive_Alias &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Alias &Directive_Alias::operator=(const Directive_Alias &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Alias::~Directive_Alias(void)
{
	return ;
}

void Directive_Alias::check_and_add(std::vector<std::string> key_val,
									std::vector<std::string> &root)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_allowed_values();
	this->check_for_duplicate_identifier(this->_config_defs);
	if (root.size() > 0)
		throw AliasNotAllowedWithRoot(this->get_directive_name());
	this->_config_defs.push_back(this->_key_val[1]);
}

std::vector<std::string> &Directive_Alias::get_config_defs(void)
{
	return (this->_config_defs);
}
