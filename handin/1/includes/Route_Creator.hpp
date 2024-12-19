/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route_Creator.hpp                                   :+:      :+:    :+:   */
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
#include "webserv.hpp"
#include "util.hpp"

class Route_Creator
{
  public:
	Route_Creator();
	Route_Creator(const Route_Creator &other);
	Route_Creator &operator=(const Route_Creator &other);
	virtual ~Route_Creator(void);

	void create_routes(Server &new_server, Server_Parser &cur_parser);

  private:
	void route_assign_location(Route &new_route, Location_Parser &cur_parser);
	void route_assign_root(Route &new_route, Location_Parser &cur_parser);
	void route_assign_autoindex(Route &new_route, Location_Parser &cur_parser);
	void route_assign_client_max_body_size(Route &new_route, Location_Parser &cur_parser);
	void route_assign_alias(Route &new_route, Location_Parser &cur_parser);
	void route_assign_allowed_methods(Route &new_route, Location_Parser &cur_parser);
	void route_assign_error_pages(Route &new_route, Location_Parser &cur_parser);
	void route_assign_index(Route &new_route, Location_Parser &cur_parser);
	void route_assign_return(Route &new_server, Location_Parser &cur_parser);
	void route_assign_cgi(Route &new_server, Location_Parser &cur_parser);
};
