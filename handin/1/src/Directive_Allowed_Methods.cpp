#include "Directive_Allowed_Methods.hpp"
#include "webserv.hpp"

Directive_Allowed_Methods::Directive_Allowed_Methods()
{
	this->_directive_name = "allowed_methods";
	this->_duplicate_definition_allowed = true;
	this->_many_args_allowed = true;
	this->_allowed_values.push_back("GET");
	this->_allowed_values.push_back("POST");
	this->_allowed_values.push_back("DELETE");
	return ;
}

Directive_Allowed_Methods::Directive_Allowed_Methods(const Directive_Allowed_Methods &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Allowed_Methods &Directive_Allowed_Methods::operator=(const Directive_Allowed_Methods &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Allowed_Methods::~Directive_Allowed_Methods(void)
{
	return ;
}

void Directive_Allowed_Methods::check_and_add(std::vector<std::string> key_val)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_allowed_values();
	std::vector<std::string>::iterator it_key_val;
	for (it_key_val = key_val.begin()
			+ 1; it_key_val != key_val.end(); it_key_val++)
	{
		if (*it_key_val == "GET")
			this->_config_defs["get"] = true;
		else if (*it_key_val == "POST")
			this->_config_defs["post"] = true;
		else if (*it_key_val == "DELETE")
			this->_config_defs["delete"] = true;
		else
			throw UnallowedArgumentForDirective(this->_directive_name);
	}

}

std::map<std::string, bool> &Directive_Allowed_Methods::get_config_defs(void)
{
	return (this->_config_defs);
}
