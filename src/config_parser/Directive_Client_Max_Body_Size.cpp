#include "Directive_Client_Max_Body_Size.hpp"
#include "webserv.hpp"

Directive_Client_Max_Body_Size::Directive_Client_Max_Body_Size()
{
	this->_directive_name = "client_max_body_size";
	this->_duplicate_definition_allowed = false;
	this->_many_args_allowed = false;
	return ;
}

Directive_Client_Max_Body_Size::Directive_Client_Max_Body_Size(const Directive_Client_Max_Body_Size &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Client_Max_Body_Size &Directive_Client_Max_Body_Size::operator=(const Directive_Client_Max_Body_Size &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Client_Max_Body_Size::~Directive_Client_Max_Body_Size(void)
{
	return ;
}

void Directive_Client_Max_Body_Size::check_and_add(std::vector<std::string> key_val)
{
	uint	size_in_bytes;

	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	this->check_for_duplicate_identifier(this->_config_defs);
	if (!this->check_body_size_format(key_val[1], size_in_bytes))
		throw InvalidClientMaxBodySizeFormat(this->get_directive_name());
	this->_config_defs.push_back(size_in_bytes);
}

std::vector<uint> &Directive_Client_Max_Body_Size::get_config_defs(void)
{
	return (this->_config_defs);
}


bool Directive_Client_Max_Body_Size::check_body_size_format(std::string size_str,
											uint &size_in_bytes)
{
	char	last_char;
	int		size_int;

	std::string num_str;
	last_char = size_str[size_str.size() - 1];
	if (isdigit(last_char))
	{
		num_str = size_str;
	}
	else
	{
		if (last_char != 'M' && last_char != 'm' && last_char != 'K'
			&& last_char != 'k' && last_char != 'G' && last_char != 'g')
			return (false);
		num_str = size_str.substr(0, size_str.length() - 1);
	}
	if (!util::is_digits_only(num_str))
		return (false);
	size_int = atoi(num_str.c_str());
	if (size_int == -1)
		return (false);
	if (last_char == 'g' || last_char == 'G')
	{
		if (size_int > 2)
			return (false);
		size_in_bytes = size_int * 1024 * 1024 * 1024;
	}
	else if (last_char == 'm' || last_char == 'M')
	{
		if (size_int > 2048)
			return (false);
		size_in_bytes = size_int * 1024 * 1024;
	}
	else if (last_char == 'k' || last_char == 'K')
	{
		if (size_int > 2097152)
			return (false);
		size_in_bytes = size_int * 1024;
	}
	else
	{
		if (size_int == INT_MAX)
			return (false);
		size_in_bytes = size_int;
	}
	return (true);
}

Directive_Client_Max_Body_Size::InvalidClientMaxBodySizeFormat::InvalidClientMaxBodySizeFormat(std::string directive_name)
	: _directive_name(directive_name)
{
}

Directive_Client_Max_Body_Size::InvalidClientMaxBodySizeFormat::~InvalidClientMaxBodySizeFormat() throw()
{
}

const char *Directive_Client_Max_Body_Size::InvalidClientMaxBodySizeFormat::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg
		+= "The provided argument for client_max_body is in the wrong format or the maximum value of 2GB is exceded!";
	return (this->_msg.c_str());
}
