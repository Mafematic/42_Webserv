#pragma once
#include "Directive.hpp"
#include "webserv.hpp"
#include "util.hpp"

class Directive_Index : public Directive
{
  public:
	Directive_Index();
	Directive_Index(const Directive_Index &other);
	Directive_Index &operator=(const Directive_Index &other);
	virtual ~Directive_Index(void);

	void check_and_add(std::vector<std::string> key_val);

	std::vector<std::string> &get_config_defs(void);

  private:
	std::vector<std::string> _config_defs;
};
