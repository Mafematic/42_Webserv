#pragma once
#include "Directive.hpp"
#include "external.hpp"
#include "util.hpp"

class Directive_Client_Max_Body_Size : public Directive
{
  public:
	Directive_Client_Max_Body_Size();
	Directive_Client_Max_Body_Size(const Directive_Client_Max_Body_Size &other);
	Directive_Client_Max_Body_Size &operator=(const Directive_Client_Max_Body_Size &other);
	virtual ~Directive_Client_Max_Body_Size(void);

	void check_and_add(std::vector<std::string> key_val);

	std::vector<uint> &get_config_defs(void);
	bool check_body_size_format(std::string size_str, uint &size_in_bytes);

	class InvalidClientMaxBodySizeFormat : public std::exception
	{
		public:
		InvalidClientMaxBodySizeFormat(std::string directive_name);
		virtual ~InvalidClientMaxBodySizeFormat() throw();
		virtual const char *what() const throw();

		private:
		std::string _directive_name;
		mutable std::string _msg;
	};

  private:
	std::vector<uint> _config_defs;
};
