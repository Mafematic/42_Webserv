#ifndef REQUESTROUTER_HPP
#define REQUESTROUTER_HPP

#include <string>
#include "Request.hpp"
#include "Server.hpp"

class RequestRouter
{
public:
    // Route a validated Request and return an appropriate HTTP response
    static std::string route(const Request &request, const Server &server);
    static Route _getRoute(const Server &server, const Request &req);

private:

    static std::string _serveFile(const std::string &filepath, int statusCode, const Request &req);
    

};

#endif
