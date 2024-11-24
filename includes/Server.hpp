#pragma once

#include "Route.hpp"
#include "util.hpp"
#include "webserv.hpp"

class Server
{
  public:
	Server(void);
	Server(const Server &other);
	Server &operator=(const Server &other);
	virtual ~Server(void);

	void set_listen(int port, std::string ip);
	void set_root(std::string new_val);
	void set_autoindex(bool new_val);
	void set_client_max_body_size(uint new_val);
	void set_index(std::vector<std::string> new_val);
	void set_server_name(std::vector<std::string> new_val);
	void set_error_pages(std::map<std::string,
		std::vector<std::string> > new_val);
	void set_routes(std::vector<Route> new_val);
	void set_return(int http_code, std::string url);
	void set_return_is_defined(bool new_val);
	void add_route(Route new_val);

	util::Address get_listen() const;
	std::string get_ip() const;
	int get_port() const;
	std::string get_root() const;
	bool get_autoindex() const;
	uint get_client_max_body_size() const;
	std::vector<std::string> get_index() const;
	std::vector<std::string> get_server_name() const;
	std::map<std::string, std::vector<std::string> > get_error_pages() const;
	std::vector<Route> get_routes() const;
	bool get_return_is_defined() const;
	util::Return_Definition get_return() const;

  private:
	util::Address _listen;
	std::string _root;
	bool _autoindex;
	uint _client_max_body_size;
	bool _return_is_defined;
	util::Return_Definition _return ;
	std::vector<std::string> _index;
	std::vector<std::string> _server_name;
	std::map<std::string, std::vector<std::string> > _error_pages;
	std::vector<Route> _routes;
};

std::ostream &operator<<(std::ostream &os, Server const &server);
