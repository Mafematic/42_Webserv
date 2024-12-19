#pragma once
#include "Directive.hpp"
#include "webserv.hpp"
#include "util.hpp"

class Directive_Listen : public Directive
{
  public:
	Directive_Listen();
	Directive_Listen(const Directive_Listen &other);
	Directive_Listen &operator=(const Directive_Listen &other);
	virtual ~Directive_Listen(void);

	void check_and_add(std::vector<std::string> key_val);
	void process_port_n_ip(std::string port_n_ip);

	void verify_port_str(std::string port_str, int &port);
	void verify_ip_str(std::string &ip_str);
	void check_port_n_ip_basic_form(std::string port_n_ip,
									std::string &port_str,
									std::string &ip_str);
	void 	check_port_address_combination_use(util::Address &new_address);

	std::vector<util::Address> &get_config_defs(void);

	class InvalidListenArgumentFormat : public std::exception
	{
		public:
		InvalidListenArgumentFormat(std::string argument);
		virtual ~InvalidListenArgumentFormat() throw();
		virtual const char *what() const throw();

		private:
		std::string _argument;
		mutable std::string _msg;
	};

	class DuplicateListenDirective : public std::exception
	{
		public:
		DuplicateListenDirective(int port, std::string ip);
		virtual ~DuplicateListenDirective() throw();
		virtual const char *what() const throw();

		private:
		int _port;
		std::string _ip;
		mutable std::string _msg;
	};

  private:
	std::vector<std::string> _config_defs;
	std::vector<util::Address> _ips_n_ports;
};
