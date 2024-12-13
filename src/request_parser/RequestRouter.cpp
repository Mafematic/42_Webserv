#include "RequestRouter.hpp"
#include "Uploader.hpp"
#include "util.hpp"
#include "webserv.hpp"
#include "Server.hpp"

#include <fstream>     // For std::ifstream
#include <string>

std::string getCustomErrorPage(const std::string &rootPath, const Route &route, int statusCode, const Server &server)
{
	std::string code = util::to_string(statusCode);

	// 1. Check for location-specific error page
	std::map<std::string, std::vector<std::string> > routeErrorPages = route.get_error_pages();
	if (routeErrorPages.count(code))
	{
		std::string errorPagePath = route.get_location() + routeErrorPages[code][0];
		if (util::fileExists(rootPath + errorPagePath))
		{
			return rootPath + errorPagePath;
		}
	}

	// 2. Check for server-level error page
	std::map<std::string, std::vector<std::string> > serverErrorPages = server.get_error_pages();
	if (serverErrorPages.count(code))
	{
		std::string errorPagePath = serverErrorPages[code][0];
		if (util::fileExists(rootPath + errorPagePath))
		{
			return rootPath + errorPagePath;
		}
	}

	// 3. Fallback to default error page
	return "./default_pages/" + code + ".html";
}



std::string generateAutoindexListing(const std::string &directoryPath, const std::string &requestPath)
{
	std::ostringstream html;

	html << "<!DOCTYPE html><html><head><title>Index of " << requestPath << "</title></head><body>";
	html << "<h1>Index of " << requestPath << "</h1><ul>";
	
	std::cout << "++++ Request path: " << requestPath << std::endl;
	std::cout << "++++ Directory path: " << directoryPath << std::endl;

	DIR *dir = opendir(directoryPath.c_str());
	if (dir)
	{
		std::cout << "+++in here3" << std::endl; 
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			// Skip "." and ".."
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
				continue;

			// Ensure no double slash in the URL
			std::string linkPath = requestPath;
			if (linkPath[linkPath.size() - 1] != '/')
			{
				linkPath += "/";
			}
			linkPath += entry->d_name;
			std::cout << "++++ Linkpath: " << linkPath << std::endl;

			html << "<li><a href=\"" << linkPath << "\">" << entry->d_name << "</a></li>";
		}
		closedir(dir);
	}
	else
	{
		html << "<p>Unable to open directory.</p>";
		return html.str();  // Return in case of error
	}

	html << "</ul></body></html>";
	return html.str();
}

std::string RequestRouter::route(Request &req, const Server &server)
{
	std::string customError;

    Route route = _getRoute(server, req);
    std::string rootPath = server.get_final_root(route);

	std::string method = req.getMethod();
	for (size_t i = 0; i < method.size(); ++i)
	{
		method[i] = std::tolower(method[i]);
	}

    if (!route.is_method_allowed(method))
	{
        customError = getCustomErrorPage(rootPath, route, 405, server);
        return _serveFile(customError, 405, req);
    }

	std::string path = req.getPath();
	if (route.get_alias_is_defined())
	{
		std::string location = route.get_location();
		if (path.find(location) == 0)
		{
			path.replace(0, location.length(), "");
			if (!path.empty() && path[0] == '/')
			{
				path = path.substr(1);
			}
		}
	}

	// Construct the final filepath
	std::string filepath = rootPath;
	if (filepath[filepath.length() - 1] != '/')
	{
		filepath += "/";
	}
	filepath += path;

	std::cout << "++++ rootPath: " << rootPath << std::endl;
	std::cout << "++++ Path: " << req.getPath() << std::endl;
	std::cout << "++++ Final Filepath: " << filepath << std::endl;
    std::cout << "++++ Autoindex: " << server.get_autoindex() << std::endl;
    std::cout << "++++ Autoindex: " << route.get_autoindex() << std::endl;

    // Test #1
	if (!req.isValid()) // Early exit for invalid requests
	{
		customError = getCustomErrorPage(rootPath, route, 400, server);
        return _serveFile(customError, 400, req);
	}
	// // Test #2
	// if (!util::directoryExists(rootPath))
	// {
	// 	customError = getCustomErrorPage(route, 500);
    //     return _serveFile(customError, 500, req); // Custom error page for invalid root
	// }

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
			customError = getCustomErrorPage(rootPath, route, 413, server);
        	return _serveFile(customError, 413, req); // Payload too large
		}

		FileUploader uploader(req);
		if (uploader.isMalformed())
		{
			// Test #4
			customError = getCustomErrorPage(rootPath, route, 400, server);
        	return _serveFile(customError, 400, req); // Malformed request
		}

		if (uploader.handleRequest())
		{
			// Test #5 - NO ERROR Code
			std::string redirectPage = getCustomErrorPage(rootPath, route, 303, server);
    		return _serveFile(redirectPage, 303, req);
		}
		// Test #6 
		customError = getCustomErrorPage(rootPath, route, 500, server);
        return _serveFile(customError, 500, req); // Internal server error
	}

	if (req.getMethod() == "GET")
	{
		if (req.getPath() == "/" || req.getPath()[req.getPath().length() - 1] == '/')
		{
			std::vector<std::string> indices = route.get_index();
			if (!indices.empty())
			{
				for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
				{
					std::string indexFilepath = filepath;
					if (indexFilepath[indexFilepath.length() - 1] != '/')
					{
						indexFilepath += "/";
					}
					indexFilepath += *it;

					std::cout << "++++ indexFile: " << indexFilepath << std::endl;

					if (util::fileExists(indexFilepath))
					{
						return _serveFile(indexFilepath, 200, req);
					}
				}
			}

			// If autoindex is enabled in the route, generate directory listing
			if (route.get_autoindex() && util::directoryExists(filepath))
			{
				if (access(filepath.c_str(), R_OK) == 0)
				{
					std::string listing = generateAutoindexListing(filepath, req.getPath());
					return _serveFile(listing, 200, req);
				}
				else
				{
					customError = getCustomErrorPage(rootPath, route, 403, server);
					return _serveFile(customError, 403, req);
				}
			}

			// If no location-specific index files and autoindex is off, fall back to server-level index files
			indices = server.get_index();
			if (!indices.empty())
			{
				for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
				{
					std::string indexFilepath = filepath;
					if (indexFilepath[indexFilepath.length() - 1] != '/')
					{
						indexFilepath += "/";
					}
					indexFilepath += *it;

					if (util::fileExists(indexFilepath))
					{
						return _serveFile(indexFilepath, 200, req);
					}
				}
			}

			// No index found, return a 404 error
			customError = getCustomErrorPage(rootPath, route, 404, server);
			return _serveFile(customError, 404, req);
		}
		else
		{
			// Handle other paths using the constructed filepath
			if (util::fileExists(filepath))
			{
				return _serveFile(filepath, 200, req);
			}

			// File or directory not found, return a 404 error
			customError = getCustomErrorPage(rootPath, route, 404, server);
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
				customError = getCustomErrorPage(rootPath, route, 500, server);
				return _serveFile(customError, 500, req);
			}
		}
		else
		{
			customError = getCustomErrorPage(rootPath, route, 404, server);
			return _serveFile(customError, 404, req); // File not found
   		}
	}

	customError = getCustomErrorPage(rootPath, route, 404, server);
	return _serveFile(customError, 404, req);
}

std::string RequestRouter::_serveFile(const std::string &contentOrFilepath, int statusCode, Request &req)
{
    std::string content = "";
    if (statusCode != 303 && req.getMethod() != "DELETE") // No body for 303
    {
        if (util::fileExists(contentOrFilepath))
        {
            std::ifstream file(contentOrFilepath.c_str());
            if (file.is_open())
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file.close();
                content = buffer.str();
            }
        }
        else
        {
            content = contentOrFilepath;  // Assume it's raw HTML content
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
		case 403:
        	statusLine = "HTTP/1.1 403 Forbidden";
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
	//statusLine += "\r\nContent-Type: image/png";

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

    //std::cout << "+++ Response: " << statusLine << std::endl;

    return statusLine;
}



Route RequestRouter::_getRoute(const Server &server, Request &req)
{

	//"/test/hallo/"

	//"./" "./root" "./test/"
    std::vector<Route> routes = server.get_routes();
    std::string path = req.getPath();

    // 1. Check for an exact match
    for (size_t i = 0; i < routes.size(); i++)
    {
        if (path == routes[i].get_location())
        {
            return routes[i];
        }
    }
    // 2. Check for the longest prefix match
    Route *bestMatch = NULL;
    for (size_t i = 0; i < routes.size(); i++)
    {
        if (path.find(routes[i].get_location()) == 0)
        {
            if (!bestMatch || routes[i].get_location().length() > bestMatch->get_location().length())
            {
                bestMatch = &routes[i];
            }
        }
    }
	//std::cout << "+++ Best Match:" << bestMatch << std::endl;

    if (bestMatch)
    {
        return *bestMatch;
    }
    return Route();
}

