
#include "Directive_Server_Name.hpp"
#include "webserv.hpp"

Directive_Server_Name::Directive_Server_Name()
{
	this->_directive_name = "server_name";
	this->_duplicate_definition_allowed = true;
	this->_many_args_allowed = true;
	return ;
}

Directive_Server_Name::Directive_Server_Name(const Directive_Server_Name &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Server_Name &Directive_Server_Name::operator=(const Directive_Server_Name &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Server_Name::~Directive_Server_Name(void)
{
	return ;
}

void Directive_Server_Name::check_and_add(std::vector<std::string> key_val)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	util::vector_append_from_index(this->_config_defs, key_val, 1);
}

std::vector<std::string> &Directive_Server_Name::get_config_defs(void)
{
	return (this->_config_defs);
}
