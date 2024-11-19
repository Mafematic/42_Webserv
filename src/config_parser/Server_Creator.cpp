/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Creator.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 14:55:39 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Creator.hpp"
#include "webserv.hpp"

Server_Creator::Server_Creator()
{
	return ;
}

Server_Creator::Server_Creator(const Server_Creator &other)
{
	(void)other;
	return ;
}

Server_Creator &Server_Creator::operator=(const Server_Creator &other)
{
	if (this != &other)
	{
		;
	}
	return (*this);
}

Server_Creator::~Server_Creator(void)
{
	return ;
}

std::vector<Server> Server_Creator::create_servers(std::vector<Server_Parser> server_parser)
{
	Server			new_server;
	Route_Creator	route_creator;

	std::vector<Server> servers;
	for (std::vector<Server_Parser>::iterator it = server_parser.begin(); it != server_parser.end(); ++it)
	{
		new_server = Server();
		this->server_assign_root(new_server, *it);
		this->server_assign_autoindex(new_server, *it);
		this->server_assign_client_max_body_size(new_server, *it);
		this->server_assign_index(new_server, *it);
		this->server_assign_server_name(new_server, *it);
		this->server_assign_error_pages(new_server, *it);
		this->server_assign_return(new_server, *it);
		route_creator.create_routes(new_server, *it);
		std::vector<util::Address> listen_directives = it->get_listen_handler().get_config_defs();
		if (listen_directives.size() == 0)
			servers.push_back(new_server);
		else
		{
			for (std::vector<util::Address>::iterator it_add = listen_directives.begin(); it_add != listen_directives.end(); ++it_add)
			{
				new_server.set_listen(it_add->port, it_add->ip);
				servers.push_back(new_server);
			}
		}
	}
	// for (std::vector<Server>::iterator vec_it = servers.begin(); vec_it != servers.end(); ++vec_it)
	// {
	// 	std::cout << *vec_it;
	// }
	return (servers);
}

void Server_Creator::server_assign_root(Server &new_server,
										Server_Parser &cur_parser)
{
	std::vector<std::string> config_defs = cur_parser.get_root_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_server.set_root(config_defs[0]);
}

void Server_Creator::server_assign_autoindex(Server &new_server,
												Server_Parser &cur_parser)
{
	std::vector<bool> config_defs = cur_parser.get_autoindex_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_server.set_autoindex(config_defs[0]);
}

void Server_Creator::server_assign_client_max_body_size(Server &new_server,
														Server_Parser &cur_parser)
{
	std::vector<uint> config_defs = cur_parser.get_client_max_body_size_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_server.set_client_max_body_size(config_defs[0]);
}

void Server_Creator::server_assign_index(Server &new_server,
											Server_Parser &cur_parser)
{
	std::vector<std::string>
		config_defs = cur_parser.get_index_handler().get_config_defs();
	if (config_defs.size() > 0)
	{
		std::vector<std::string> default_index = new_server.get_index();
		config_defs.insert(config_defs.end(), default_index.begin(),
				default_index.end());
		new_server.set_index(config_defs);
	}
}

void Server_Creator::server_assign_server_name(Server &new_server,
												Server_Parser &cur_parser)
{
	std::vector<std::string>
		config_defs = cur_parser.get_server_name_handler().get_config_defs();
	if (config_defs.size() > 0)
	{
		std::vector<std::string> default_server_name = new_server.get_server_name();
		config_defs.insert(config_defs.end(),
							default_server_name.begin(),
							default_server_name.end());
		new_server.set_server_name(config_defs);
	}
}

void Server_Creator::server_assign_error_pages(Server &new_server,
												Server_Parser &cur_parser)
{
	std::map<std::string,
				std::vector<std::string> >
		default_error_pages = new_server.get_error_pages();
	std::map<std::string,
				std::vector<std::string> >
		parser_error_pages = cur_parser.get_error_page_handler().get_error_pages();
	std::map<std::string, std::vector<std::string> > new_error_pages;
	for (std::map<std::string,
					std::vector<std::string> >::iterator it = parser_error_pages.begin();
			it != parser_error_pages.end();
			++it)
	{
		new_error_pages[it->first].insert(new_error_pages[it->first].end(),
											it->second.begin(),
											it->second.end());
	}
	for (std::map<std::string,
					std::vector<std::string> >::iterator it = default_error_pages.begin();
			it != default_error_pages.end();
			++it)
	{
		new_error_pages[it->first].insert(new_error_pages[it->first].end(),
											it->second.begin(),
											it->second.end());
	}
	new_server.set_error_pages(new_error_pages);
}

void Server_Creator::server_assign_return(Server &new_server,
														Server_Parser &cur_parser)
{
	std::vector<util::Return_Definition> config_defs = cur_parser.get_return_handler().get_config_defs();
	if (config_defs.size() > 0)
		new_server.set_return(config_defs[0].status_code, config_defs[0].url);
}


