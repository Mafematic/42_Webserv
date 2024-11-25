#include "RequestRouter.hpp"
#include "Uploader.hpp"
#include "util.hpp"
#include "webserv.hpp"

#include <fstream>     // For std::ifstream
#include <string>

std::string RequestRouter::route(const Request &req, const Server &server)
{
	if (!req.isValid()) // Early exit for invalid requests
	{
		return _serveFile(server.get_root() + "/400.html", 400, req);
	}

	Route route = _getRoute(server, req);
	if (!util::directoryExists(route.get_root()))
	{
		return _serveFile("root/500.html", 500, req); // Custom error page for invalid root
	}

	if (req.getMethod() == "POST" && req.getPath() == "/upload")
	{
		int contentLength = 0;
		std::istringstream iss(req.getHeader("Content-Length"));
		if (req.getHeader("Content-Length").empty() || !(iss >> contentLength) || contentLength > MAX_UPLOAD_SIZE)
		{
			return _serveFile(server.get_root() + "/413.html", 413, req); // Payload too large
		}

		FileUploader uploader(req);
		if (uploader.isMalformed())
		{
			return _serveFile(server.get_root() + "/400.html", 400, req); // Malformed request
		}

		if (uploader.handleRequest())
		{
			return "HTTP/1.1 303 See Other\r\n"
				   "Location: /303.html\r\n"
				   "Connection: close\r\n"
				   "\r\n";
		}

		return _serveFile(server.get_root() + "/500.html", 500, req); // Internal server error
	}

	if (req.getMethod() == "GET")
	{
		if (req.getPath() == "/")
		{
			return _serveFile(route.get_root() + "/index.html", 200, req);
		}
		return _serveFile(route.get_root() + req.getPath(), 200, req);
	}

	if (req.getMethod() == "DELETE")
	{
		std::string filepath = route.get_root() + req.getPath();
		bool exists = std::ifstream(filepath.c_str()).good();
		if (exists)
		{
			if (remove(filepath.c_str()) == 0)
			{
				return _serveFile(server.get_root() + "/200.html", 200, req);
			}
			else
			{
				return _serveFile(server.get_root() +"/500.html", 500, req); // Internal server error
			}
		}
		else
		{
			return _serveFile(server.get_root() + "/404.html", 404, req); // File not found
   		}
	}

	return _serveFile(server.get_root() + "root/404.html", 404, req);
}

std::string RequestRouter::_serveFile(const std::string &filepath, int statusCode, const Request &req)
{
	std::string content = "";

	if (req.getMethod() != "DELETE")
	{
		std::ifstream file(filepath.c_str());
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			content = buffer.str();
		}
	}
	std::string statusLine;
	switch (statusCode)
	{
	case 200:
		statusLine = "HTTP/1.1 200 OK";
		break;
	case 404:
		statusLine = "HTTP/1.1 404 Not Found";
		break;
	case 500:
		statusLine = "HTTP/1.1 500 Internal Server Error";
		break;
	default:
		statusLine = "HTTP/1.1 200 OK";
	}
	statusLine += "\r\nContent-Type: text/html";
	statusLine += "\r\nContent-Length: ";
	statusLine += util::to_string(content.size());
	if (req.getKeepAlive())
	{
		statusLine += "\r\nConnection: keep-alive";
	}
	else
	{
		statusLine += "\r\nConnection: close";
	}
	statusLine += "\r\n\r\n";
	statusLine += content;
	//std::cout << "+++ Repsonse: " << statusLine << std::endl;

	return statusLine;
}

Route RequestRouter::_getRoute(const Server &server, const Request &req)
{
	std::vector<Route> routes = server.get_routes();
	std::string path = req.getPath();

	for (size_t i = 0; i < routes.size(); i++)
	{
		if (path.find(routes[i].get_location()) == 0)
		{
			return routes[i];
		}
	}
	// If no route matches, return a default route
	return Route(); // A default constructor handles unmatched cases
}