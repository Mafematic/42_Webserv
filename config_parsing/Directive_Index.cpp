#include "Directive_Index.hpp"
#include "external.hpp"

Directive_Index::Directive_Index()
{
	this->_directive_name = "index";
	this->_duplicate_definition_allowed = true;
	this->_many_args_allowed = true;
	return ;
}

Directive_Index::Directive_Index(const Directive_Index &other)
	: Directive()
{
	this->_config_defs = other._config_defs;
	return ;
}

Directive_Index &Directive_Index::operator=(const Directive_Index &other)
{
	if (this != &other)
	{
		this->_config_defs = other._config_defs;
	}
	return (*this);
}

Directive_Index::~Directive_Index(void)
{
	return ;
}

void Directive_Index::check_and_add(std::vector<std::string> key_val)
{
	this->set_key_val(key_val);
	this->check_for_invalid_num_args();
	util::vector_append_from_index(this->_config_defs, key_val, 1);
}

std::vector<std::string> &Directive_Index::get_config_defs(void)
{
	return (this->_config_defs);
}