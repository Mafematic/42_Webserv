#ifndef REQUESTROUTER_HPP
#define REQUESTROUTER_HPP

#include <string>
#include "Request.hpp"
#include "Server.hpp"
#include "Client.hpp"

class RequestRouter
{
public:
    static bool valid;
    // Route a validated Request and return an appropriate HTTP response
    static std::string route(Request &request, const Server &server);
    static Route _getRoute(const Server &server, Request &req);
    static std::string getCustomErrorPage(const std::string &rootPath, const Route &route, int statusCode, const Server &server);
    static std::string _serveFile(const std::string &filepath, int statusCode, Request &req);
private:


};

#endif
