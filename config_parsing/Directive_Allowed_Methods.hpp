#pragma once
#include "Directive.hpp"
#include "external.hpp"
#include "util.hpp"

class Directive_Allowed_Methods : public Directive
{
  public:
	Directive_Allowed_Methods();
	Directive_Allowed_Methods(const Directive_Allowed_Methods &other);
	Directive_Allowed_Methods &operator=(const Directive_Allowed_Methods &other);
	virtual ~Directive_Allowed_Methods(void);

	void check_and_add(std::vector<std::string> key_val);

	std::map<std::string, bool> &get_config_defs(void);


  private:
	std::map<std::string, bool> _config_defs;
};
