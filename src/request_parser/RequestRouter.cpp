#include "RequestRouter.hpp"
#include "Uploader.hpp"
#include "util.hpp"
#include "webserv.hpp"
#include "Server.hpp"

#include <fstream>     // For std::ifstream
#include <string>

std::string getCustomErrorPage(const Route &route, int statusCode)
{
    std::map<std::string, std::vector<std::string> > errorPages = route.get_error_pages();
    std::string code = util::to_string(statusCode);

    if (errorPages.count(code) && util::fileExists(route.get_root() + errorPages[code][0]))
    {
        return route.get_root() + errorPages[code][0];
    }

    // Return the default error page if no custom page exists
    return "./default_pages/" + code + ".html";
}

std::string RequestRouter::route(const Request &req, const Server &server)
{
	std::string customError;
	Route route = _getRoute(server, req);
	std::cout << "++++ Root1: " << route.get_root() << std::endl; 
	std::cout << "++++ Root2: " << server.get_root() << std::endl; 

	std::cout << "++++ Path1: " << req.getPath() << std::endl; 

	std::string filepath = server.get_root() + req.getPath();
	std::cout << "++++ filepath: " << filepath << std::endl; 

	
	// Test #1
	if (!req.isValid()) // Early exit for invalid requests
	{
		customError = getCustomErrorPage(route, 400);
        return _serveFile(customError, 400, req);
	}
	// Test #2
	if (!util::directoryExists(route.get_root()))
	{
		customError = getCustomErrorPage(route, 500);
        return _serveFile(customError, 500, req); // Custom error page for invalid root
	}

	if (req.getMethod() == "POST" && req.getPath() == "/upload")
	{
		uint contentLength = 0;
		std::istringstream iss(req.getHeader("Content-Length"));
		//std::cout << "++++ client max body size: " << route.get_client_max_body_size() << std::endl; 
		//std::cout << "++++ Server / client max body size: " << server.get_client_max_body_size() << std::endl; 

		uint maxBodySize = server.get_client_max_body_size();
		if (req.getHeader("Content-Length").empty() || !(iss >> contentLength) || contentLength > maxBodySize)
		{
			// Test #3
			customError = getCustomErrorPage(route, 413);
        	return _serveFile(customError, 413, req); // Payload too large
		}

		FileUploader uploader(req);
		if (uploader.isMalformed())
		{
			// Test #4
			customError = getCustomErrorPage(route, 400);
        	return _serveFile(customError, 400, req); // Malformed request
		}

		if (uploader.handleRequest())
		{
			// Test #5 - NO ERROR Code
			std::string redirectPage = getCustomErrorPage(route, 303);
    		return _serveFile(redirectPage, 303, req);
		}
		// Test #6 
		customError = getCustomErrorPage(route, 500);
        return _serveFile(customError, 500, req); // Internal server error
	}

	if (req.getMethod() == "GET")
	{
		if (req.getPath() == "/")
		{
			std::vector<std::string> indices = route.get_index();
			indices.push_back("index.html"); // Local fallback

			for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
			{
				filepath = route.get_root() + "/" + *it;
				if (util::fileExists(filepath))
				{
					return _serveFile(filepath, 200, req); // Serve the first matching index file in the route root
				}
			}

			// Global fallback
			filepath = "./default_pages/index.html";
			if (util::fileExists(filepath))
			{
				return _serveFile(filepath, 200, req);
			}
			// Test #7
			customError = getCustomErrorPage(route, 404);
			return _serveFile(customError, 404, req);
		}
		else
		{
			// Handle other paths
			//filepath = route.get_root() + req.getPath();
			std::cout << "+++ filepath:" << std::endl;
			if (util::fileExists(filepath))
			{
				return _serveFile(filepath, 200, req);
			}
			customError = getCustomErrorPage(route, 404);
			return _serveFile(customError, 404, req);
		}
	}
	if (req.getMethod() == "DELETE")
	{
		if (util::fileExists(filepath))
		{
			if (remove(filepath.c_str()) == 0)
			{
				return _serveFile(filepath, 200, req);
			}
			else
			{
				// Test #8
				customError = getCustomErrorPage(route, 500);
				return _serveFile(customError, 500, req);
			}
		}
		else
		{
			// Test #9
			customError = getCustomErrorPage(route, 404);
			return _serveFile(customError, 404, req); // File not found
   		}
	}

	customError = getCustomErrorPage(route, 404);
	return _serveFile(customError, 404, req);
}

std::string RequestRouter::_serveFile(const std::string &filepath, int statusCode, const Request &req)
{
    std::string content = "";
    if (statusCode != 303 && req.getMethod() != "DELETE") // No body for 303
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
        case 303:
            statusLine = "HTTP/1.1 303 See Other";
            break;
        case 404:
            statusLine = "HTTP/1.1 404 Not Found";
            break;
        case 413:
            statusLine = "HTTP/1.1 413 Payload Too Large";
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

    if (statusCode == 303) // Include Location header for redirection
	{
		statusLine += "\r\nLocation: /303.html"; // Redirect to /303.html
	}

    if (req.getKeepAlive())
    {
        statusLine += "\r\nConnection: keep-alive";
    }
    else
    {
        statusLine += "\r\nConnection: close";
    }
    statusLine += "\r\n\r\n";
    if (statusCode != 303) // Avoid body for 303
    {
        statusLine += content;
    }
    std::cout << "+++ Response: " << statusLine << std::endl;

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

