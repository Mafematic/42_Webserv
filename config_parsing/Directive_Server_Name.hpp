#pragma once
#include "Directive.hpp"
#include "external.hpp"
#include "util.hpp"

class Directive_Server_Name : public Directive
{
  public:
	Directive_Server_Name();
	Directive_Server_Name(const Directive_Server_Name &other);
	Directive_Server_Name &operator=(const Directive_Server_Name &other);
	virtual ~Directive_Server_Name(void);

	void check_and_add(std::vector<std::string> key_val);

	std::vector<std::string> &get_config_defs(void);

  private:
	std::vector<std::string> _config_defs;
};
