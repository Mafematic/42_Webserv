#include "Directive_Cgi.hpp"
#include "webserv.hpp"

Directive_Cgi::Directive_Cgi()
{
	this->_directive_name = "cgi";
	this->_duplicate_definition_allowed = true;
	this->_many_args_allowed = true;
	return ;
}

Directive_Cgi::Directive_Cgi(const Directive_Cgi &other) : Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Cgi &Directive_Cgi::operator=(const Directive_Cgi &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Cgi::~Directive_Cgi(void)
{
	return ;
}

void Directive_Cgi::check_and_add(std::vector<std::string> key_val)
{
	std::string cgi_extension;
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	if (this->_key_val.size() != 3)
		throw InvalidNumberOfArguments(this->_directive_name);
	cgi_extension = this->_key_val[1];
	if (cgi_extension[0] != '.')
		throw WrongCgiInterpreterDefinition(this->_directive_name);
	if (cgi_extension.length() < 2 || cgi_extension.length() > 5)
		throw WrongCgiInterpreterDefinition(this->_directive_name);
	if (!this->is_valid_string(cgi_extension))
		throw WrongCgiInterpreterDefinition(this->_directive_name);
	cgi_extension.erase(0, 1);
	this->_config_defs[cgi_extension] = this->_key_val[2];
}

bool Directive_Cgi::is_valid_string(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isalnum(*it) && *it != '.')
			return (false);
	}
	return (true);
}

std::map<std::string, std::string> &Directive_Cgi::get_config_defs(void)
{
	return (this->_config_defs);
}

Directive_Cgi::WrongCgiInterpreterDefinition::WrongCgiInterpreterDefinition(std::string directive_name) : _directive_name(directive_name)
{
}

Directive_Cgi::WrongCgiInterpreterDefinition::~WrongCgiInterpreterDefinition() throw()
{
}

const char *Directive_Cgi::WrongCgiInterpreterDefinition::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg
		+= "The provided cgi interpreter for a certain file extension is invalid. \n";
	this->_msg += "Syntax should be: cgi .[2-5chars] interpreter_path \n";
	return (this->_msg.c_str());
}
