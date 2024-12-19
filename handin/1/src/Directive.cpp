#include "Directive.hpp"
#include "webserv.hpp"

Directive::Directive()
	: _directive_name("default"),
		_many_args_allowed(true),
		_duplicate_definition_allowed(true)
{
	return ;
}

Directive::Directive(const Directive &other)
{
	this->_key_val = other._key_val;
	this->_directive_name = other._directive_name;
	this->_many_args_allowed = other._many_args_allowed;
	this->_duplicate_definition_allowed = other._duplicate_definition_allowed;
	this->_allowed_values = other._allowed_values;
	return ;
}

Directive &Directive::operator=(const Directive &other)
{
	if (this != &other)
	{
		this->_key_val = other._key_val;
		this->_directive_name = other._directive_name;
		this->_many_args_allowed = other._many_args_allowed;
		this->_duplicate_definition_allowed = other._duplicate_definition_allowed;
		this->_allowed_values = other._allowed_values;
	}
	return (*this);
}

Directive::~Directive(void)
{
	return ;
}

void Directive::check_for_invalid_num_args(void)
{
	if (this->_key_val.size() < 2)
		throw InvalidNumberOfArguments(this->_directive_name);
	if (!this->_many_args_allowed)
		if (this->_key_val.size() != 2)
			throw InvalidNumberOfArguments(this->_directive_name);
}

std::string Directive::get_directive_name(void) const
{
	return (this->_directive_name);
}

void Directive::set_key_val(std::vector<std::string> new_key_val)
{
	this->_key_val = new_key_val;
}

void Directive::check_for_allowed_values(void)
{
	if (this->_allowed_values.size() > 0)
	{
		for (std::vector<std::string>::iterator it = this->_key_val.begin()
				+ 1; it != this->_key_val.end(); ++it)
		{
			if (std::find(this->_allowed_values.begin(),
							this->_allowed_values.end(),
							*it) == this->_allowed_values.end())
				throw UnallowedArgumentForDirective(this->_directive_name);
		}
	}
}

Directive::DuplicateIdentifier::DuplicateIdentifier(std::string directive_name)
	: _directive_name(directive_name)
{
}

Directive::DuplicateIdentifier::~DuplicateIdentifier() throw()
{
}

const char *Directive::DuplicateIdentifier::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg += "Has been defined more than once inside a block!\n";
	return (this->_msg.c_str());
}

Directive::InvalidNumberOfArguments::InvalidNumberOfArguments(std::string directive_name)
	: _directive_name(directive_name)
{
}

Directive::InvalidNumberOfArguments::~InvalidNumberOfArguments() throw()
{
}

const char *Directive::InvalidNumberOfArguments::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg += "An invalid number of arguments has been provided!\n";
	return (this->_msg.c_str());
}

Directive::UnallowedArgumentForDirective::UnallowedArgumentForDirective(std::string directive_name)
	: _directive_name(directive_name)
{
}

Directive::UnallowedArgumentForDirective::~UnallowedArgumentForDirective() throw()
{
}

const char *Directive::UnallowedArgumentForDirective::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg += "An unallowed argument has been provided for the directive!\n";
	return (this->_msg.c_str());
}

Directive::AliasNotAllowedWithRoot::AliasNotAllowedWithRoot(std::string directive_name)
	: _directive_name(directive_name)
{
}

Directive::AliasNotAllowedWithRoot::~AliasNotAllowedWithRoot() throw()
{
}

const char *Directive::AliasNotAllowedWithRoot::what() const throw()
{
	this->_msg = "directive: ";
	this->_msg += this->_directive_name;
	this->_msg += "\n";
	this->_msg
		+= "Either the root or alias keyword can be defined inside a location block,not both!\n";
	return (this->_msg.c_str());
}
