#ifndef REQUESTROUTER_HPP
#define REQUESTROUTER_HPP

#include <string>
#include "Request.hpp"
#include "Server.hpp"
#include "Client.hpp"

class RequestRouter
{
public:
    // Route a validated Request and return an appropriate HTTP response
    static std::string route(Request &request, const Server &server);
    static Route _getRoute(const Server &server, Request &req);

private:

    static std::string _serveFile(const std::string &filepath, int statusCode, Request &req);


};

#endif
