/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Creator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smatthes  <smatthes@student.42berlin>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 08:42:13 by smatthes          #+#    #+#             */
/*   Updated: 2024/11/11 14:51:20 by smatthes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Route.hpp"
#include "Server.hpp"
#include "Server_Parser.hpp"
#include "Route_Creator.hpp"
#include "webserv.hpp"
#include "util.hpp"

class Server_Creator
{
  public:
	Server_Creator();
	Server_Creator(const Server_Creator &other);
	Server_Creator &operator=(const Server_Creator &other);
	virtual ~Server_Creator(void);

	std::vector<Server> create_servers(std::vector<Server_Parser> server_parser);

  private:
	void server_assign_root(Server &new_server, Server_Parser &cur_parser);
	void server_assign_return(Server &new_server, Server_Parser &cur_parser);
	void server_assign_autoindex(Server &new_server, Server_Parser &cur_parser);
	void server_assign_client_max_body_size(Server &new_server, Server_Parser &cur_parser);
	void server_assign_index(Server &new_server, Server_Parser &cur_parser);
	void server_assign_server_name(Server &new_server, Server_Parser &cur_parser);
	void server_assign_error_pages(Server &new_server, Server_Parser &cur_parser);
};
