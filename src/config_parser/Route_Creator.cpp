/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route_Creator.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route_Creator.hpp"
#include "webserv.hpp"

Route_Creator::Route_Creator()
{
	return ;
}

Route_Creator::Route_Creator(const Route_Creator &other)
{
	(void)other;
	return ;
}

Route_Creator &Route_Creator::operator=(const Route_Creator &other)
{
	if (this != &other)
	{
		;
	}
	return (*this);
}

Route_Creator::~Route_Creator(void)
{
	return ;
}
void Route_Creator::create_routes(Server &new_server, Server_Parser &cur_parser)
{
	Route	new_route;

	std::vector<Location_Parser> location_parsers = cur_parser.get_location_parser();
	for (std::vector<Location_Parser>::iterator it = location_parsers.begin(); it != location_parsers.end(); ++it)
	{
		new_route = Route();
		this->route_assign_location(new_route, *it);
		this->route_assign_root(new_route, *it);
		this->route_assign_autoindex(new_route, *it);
		this->route_assign_client_max_body_size(new_route, *it);
		this->route_assign_alias(new_route, *it);
		this->route_assign_allowed_methods(new_route, *it);
		this->route_assign_index(new_route, *it);
		this->route_assign_error_pages(new_route, *it);
		this->route_assign_return(new_route, *it);
		this->route_assign_cgi(new_route, *it);
		new_server.add_route(new_route);
	}
}

void Route_Creator::route_assign_location(Route &new_route,
	Location_Parser &cur_parser)
{
	new_route.set_location(cur_parser.get_location());
}

void Route_Creator::route_assign_root(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<std::string> config_defs = cur_parser.get_root_handler().get_config_defs();
	std::cout << config_defs.size() << std::endl;
	if (config_defs.size() > 0)
	{
		std::cout << config_defs[0] << std::endl;
		new_route.set_root(config_defs[0]);
	}
}

void Route_Creator::route_assign_autoindex(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<bool> config_defs = cur_parser.get_autoindex_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_route.set_autoindex(config_defs[0]);
}

void Route_Creator::route_assign_client_max_body_size(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<uint> config_defs = cur_parser.get_client_max_body_size_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_route.set_client_max_body_size(config_defs[0]);
}

void Route_Creator::route_assign_alias(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<std::string> config_defs = cur_parser.get_alias_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_route.set_alias(config_defs[0]);
}

void Route_Creator::route_assign_allowed_methods(Route &new_route,
	Location_Parser &cur_parser)
{
	std::map<std::string,
		bool> config_defs = cur_parser.get_allowed_methods_handler().get_config_defs();
	if (config_defs.size() > 0)
	{
		std::map<std::string,
			bool> default_allowed_methods = new_route.get_allowed_methods();
		for (std::map<std::string,
			bool>::iterator it = default_allowed_methods.begin(); it != default_allowed_methods.end(); ++it)
		{
			std::map<std::string,
				bool>::iterator it_find = config_defs.find(it->first);
			if (it_find == config_defs.end())
				config_defs[it->first] = false;
		}
		new_route.set_allowed_methods(config_defs);
	}
}

void Route_Creator::route_assign_index(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<std::string> config_defs = cur_parser.get_index_handler().get_config_defs();
	if (config_defs.size() > 0)
	{
		std::vector<std::string> default_index = new_route.get_index();
		config_defs.insert(config_defs.end(), default_index.begin(),
			default_index.end());
		new_route.set_index(config_defs);
	}
}

void Route_Creator::route_assign_error_pages(Route &new_route,
	Location_Parser &cur_parser)
{
	std::map<std::string,
		std::vector<std::string> > default_error_pages = new_route.get_error_pages();
	std::map<std::string,
		std::vector<std::string> > parser_error_pages = cur_parser.get_error_page_handler().get_error_pages();
	std::map<std::string, std::vector<std::string> > new_error_pages;
	for (std::map<std::string,
		std::vector<std::string> >::iterator it = parser_error_pages.begin(); it != parser_error_pages.end(); ++it)
	{
		new_error_pages[it->first].insert(new_error_pages[it->first].end(),
			it->second.begin(), it->second.end());
	}
	for (std::map<std::string,
		std::vector<std::string> >::iterator it = default_error_pages.begin(); it != default_error_pages.end(); ++it)
	{
		new_error_pages[it->first].insert(new_error_pages[it->first].end(),
			it->second.begin(), it->second.end());
	}
	new_route.set_error_pages(new_error_pages);
}

void Route_Creator::route_assign_return(Route &new_route,
	Location_Parser &cur_parser)
{
	std::vector<util::Return_Definition> config_defs = cur_parser.get_return_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_route.set_return(config_defs[0].status_code, config_defs[0].url);
}


void Route_Creator::route_assign_cgi(Route &new_route,
	Location_Parser &cur_parser)
{
	std::map<std::string, std::string> config_defs = cur_parser.get_cgi_handler().get_config_defs();
	for (std::map<std::string, std::string>::iterator it = config_defs.begin(); it != config_defs.end(); ++it)
	{
		new_route.set_cgi_interpreter(it->first, it->second);
	}
}
