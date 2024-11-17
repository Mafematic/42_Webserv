/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:35:59 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/15 16:47:47 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
#include "util.hpp"

class Route
{
  public:
	Route(void);
	Route(const Route &other);
	Route &operator=(const Route &other);
	virtual ~Route(void);

	std::string get_location() const;
	std::string get_root() const;
	bool get_return_is_defined() const;
	util::Return_Definition get_return() const;
	bool get_alias_is_defined() const;
	std::string get_alias() const;
	uint get_client_max_body_size() const;
	bool get_autoindex() const;
	std::vector<std::string> get_error_page(std::string page) const;
	std::map<std::string, bool> get_allowed_methods() const;
	std::map<std::string, std::vector<std::string> > get_error_pages() const;
	std::vector<std::string> get_index() const;


	void set_location(std::string new_val);
	void set_root(std::string new_val);
	void set_alias(std::string new_val);
	void set_alias_is_defined(bool new_val);
	void set_return(int http_code, std::string url);
	void set_return_is_defined(bool new_val);
	void set_client_max_body_size(uint new_val);
	void set_autoindex(bool new_val);
	void set_allowed_methods(std::map<std::string, bool> new_val);
	void set_index(std::vector<std::string> new_val);
	void set_error_pages(std::map<std::string,
									std::vector<std::string> > new_val);

	bool is_method_allowed(std::string method);

  private:
	std::string _location;
	std::string _root;
	bool _alias_is_defined;
	std::string _alias;
	bool _return_is_defined;
	util::Return_Definition _return;
	uint _client_max_body_size;
	bool _autoindex;
	std::map<std::string, bool> _allowed_methods;
	// search for index files at runtime, in case one is deleted
	std::vector<std::string> _index;
	// search for error pages at runtime, in case one is deleted
	std::map<std::string, std::vector<std::string> > _error_pages;
};

std::ostream &operator<<(std::ostream &os, Route const &route);
