#pragma once
#include "Directive.hpp"
#include "util.hpp"
#include "webserv.hpp"

class Directive_Cgi : public Directive
{
  public:
	Directive_Cgi();
	Directive_Cgi(const Directive_Cgi &other);
	Directive_Cgi &operator=(const Directive_Cgi &other);
	virtual ~Directive_Cgi(void);

	void check_and_add(std::vector<std::string> key_val);

	std::map<std::string, std::string> &get_config_defs(void);

	class WrongCgiInterpreterDefinition : public std::exception
	{
		public:
		WrongCgiInterpreterDefinition(std::string directive_name);
		virtual ~WrongCgiInterpreterDefinition() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

  private:
	std::map<std::string, std::string> _config_defs;
	bool is_valid_string(const std::string &str);
};
