#include "RequestRouter.hpp"
#include "Uploader.hpp"
#include "util.hpp"
#include "webserv.hpp"
#include "Server.hpp"
#include "Path_Analyser.hpp"


bool RequestRouter::valid = true;

std::string RequestRouter::getCustomErrorPage(const std::string &rootPath, const Route &route, int statusCode, const Server &server)
{
	std::string code = util::to_string(statusCode);
	// 1. Check for location-specific error page
	std::map<std::string, std::vector<std::string> > routeErrorPages = route.get_error_pages();
	if (routeErrorPages.count(code))
	{
		std::string errorPagePath = routeErrorPages[code][0];
		if (util::fileExists(rootPath + "/" + errorPagePath))
		{
			//return rootPath + route.get_location() + "/" + errorPagePath;
			return rootPath + "/" + errorPagePath;
		}
	}

	// 2. Check for server-level error page
	std::map<std::string, std::vector<std::string> > serverErrorPages = server.get_error_pages();
	if (serverErrorPages.count(code))
	{
		std::string errorPagePath = serverErrorPages[code][0];
		std::string serverRoot = server.get_root();

		if (errorPagePath[0] == '/')
		{
			errorPagePath = errorPagePath.substr(1);
		}

		if (util::fileExists(serverRoot + "/" + errorPagePath))
		{
			return serverRoot + "/" + errorPagePath;
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

	DIR *dir = opendir(directoryPath.c_str());
	if (dir)
	{
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
			html << "<li><a href=\"" << linkPath << "\">" << entry->d_name << "</a></li>";
		}
		closedir(dir);
	}
	else
	{
		html << "<p>Unable to open directory.</p>";
		return html.str();
	}

	html << "</ul></body></html>";
	return html.str();
}

uint get_max_body_size(const Route &route, const Server &server)
{
	if (route.get_client_max_body_size() > 0)
	{
		return route.get_client_max_body_size();
	}

	if (server.get_client_max_body_size() > 0)
	{
		return server.get_client_max_body_size();
	}
	return 1024 * 1024; // Default to 1 MB
	}


std::string RequestRouter::route(Request &req, const Server &server)
{
	valid = true;
	std::string customError;
    if (server.get_return_is_defined())
	{
		util::Return_Definition serverReturn = server.get_return();
		return _serveFile(serverReturn.url, serverReturn.status_code, req);
	}

	Route route = _getRoute(server, req);
	std::string rootPath = server.get_final_root(route);

	// Check for route-level return directive
	if (route.get_return_is_defined())
	{
		util::Return_Definition routeReturn = route.get_return();
		return _serveFile(routeReturn.url, routeReturn.status_code, req);
	}

	std::string method = req.getMethod();
	for (size_t i = 0; i < method.size(); ++i)
	{
		method[i] = std::tolower(method[i]);
	}

    if (!route.is_method_allowed(method))
	{
		valid = false;
        customError = getCustomErrorPage(rootPath, route, 405, server);
        return _serveFile(customError, 405, req);
    }

	// alias handling
	std::string path = req.getPath();
	if (route.get_alias_is_defined())
	{
		std::string location = route.get_location();
		std::string aliasPath = route.get_alias();

		if (path.find(location) == 0)
		{
			path.replace(0, location.length(), aliasPath);

			if (!path.empty() && path[0] == '/')
			{
				path = path.substr(1);
			}

			rootPath = aliasPath;
		}
	}
	// Remove the leading slash from path if present
	if (!path.empty() && path[0] == '/')
	{
		path = path.substr(1);
	}

	std::string filepath = rootPath;
	if (filepath[filepath.length() - 1] != '/')
	{
		filepath += "/";
	}
	if (path == filepath || path.empty())
	{
		path = "";
	}

	filepath += path;

    // Test #1
	if (!req.isValid()) // Early exit for invalid requests
	{
		customError = getCustomErrorPage(filepath, route, 400, server);
        return _serveFile(customError, 400, req);
	}
	// // Test #2
	// if (!util::directoryExists(rootPath))
	// {
	// 	customError = getCustomErrorPage(route, 500);
    //     return _serveFile(customError, 500, req); // Custom error page for invalid root
	// }

	//if (req.getMethod() == "POST" && req.getPath() == "/fileupload")
	if (req.getMethod() == "POST" && req.getPath() == "/fileupload")
	{
		if (route.get_location() != "/fileupload")
		{
			valid = false;
			customError = getCustomErrorPage(filepath, route, 403, server);
			return _serveFile(customError, 403, req);
		}
		uint maxBodySize = get_max_body_size(route, server);
		uint contentLength = 0;
		std::istringstream iss(req.getHeader("Content-Length"));

		if (req.getHeader("Content-Length").empty() || !(iss >> contentLength) || contentLength > maxBodySize)
		{
			// Test #3
			customError = getCustomErrorPage(filepath, route, 413, server);
        	return _serveFile(customError, 413, req); // Payload too large
		}
		FileUploader uploader(req);

		if (!util::directory_is_writable("./uploads"))
		{
			customError = getCustomErrorPage(filepath, route, 403, server);
			return _serveFile(customError, 403, req);
		}
		if (uploader.isMalformed())
		{
			// Test #4
			customError = getCustomErrorPage(filepath, route, 400, server);
        	return _serveFile(customError, 400, req); // Malformed request
		}

		if (uploader.handleRequest())
		{
			// Test #5 - NO ERROR Code
			std::string redirectPage = getCustomErrorPage(filepath, route, 303, server);
    		return _serveFile(redirectPage, 303, req);
		}
		// Test #6
		customError = getCustomErrorPage(filepath, route, 500, server);
        return _serveFile(customError, 500, req); // Internal server error
	}

	if (req.getMethod() == "GET" || (req.getMethod() == "POST" && route.get_location() == "/cgi-bin/"))
	{
		if (req.getPath().find("/cgi-bin/") == 0)
		{
			uint maxBodySize = get_max_body_size(route, server);
			uint contentLength = 0;
			std::istringstream iss(req.getHeader("Content-Length"));

			if ((req.getHeader("Content-Length").empty() || !(iss >> contentLength) || contentLength > maxBodySize) && req.getMethod() == "POST")
			{
				valid = false;
				customError = getCustomErrorPage(filepath, route, 413, server);
				return _serveFile(customError, 413, req);
			}

			Path_Analyser pathAnalyser;
			pathAnalyser.analyse(req.getPath(), filepath);
			std::string scriptFullPath = pathAnalyser.path_translated;
			std::string cgiDir = filepath + "/cgi-bin";

			// Check if the cgi-bin directory is executable
			if (!route.is_executable(cgiDir))
			{
				valid = false;
				customError = getCustomErrorPage(filepath, route, 403, server);
				return _serveFile(customError, 403, req);
			}

			if (!util::fileExists(scriptFullPath))
			{
				valid = false;
				customError = getCustomErrorPage(filepath, route, 404, server);
				return _serveFile(customError, 404, req);
			}

			// Check if the specific CGI file is executable
			if (!route.is_executable(scriptFullPath))
			{
				valid = false;
				customError = getCustomErrorPage(filepath, route, 403, server);
				return _serveFile(customError, 403, req);
			}
			return _serveFile(scriptFullPath, 200, req);
		}
		if (req.getPath() == "/" || req.getPath()[req.getPath().length() - 1] == '/')
		{
			if (!route.is_readable(filepath))
			{
				valid = false;
				customError = getCustomErrorPage(filepath, route, 403, server);
				return _serveFile(customError, 403, req);
			}
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
					customError = getCustomErrorPage(filepath, route, 403, server);
					return _serveFile(customError, 403, req);
				}
			}

			// If no location-specific index files and autoindex is off, fall back to server-level index files
			indices = server.get_index();
			if (!indices.empty())
			{
				for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
				{
					std::string indexFilepath = server.get_root();
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
			customError = getCustomErrorPage(filepath, route, 404, server);
			return _serveFile(customError, 404, req);
		}
		else
		{
			if (util::fileExists(filepath))
			{
				return _serveFile(filepath, 200, req);
			}
			customError = getCustomErrorPage(filepath, route, 404, server);
			return _serveFile(customError, 404, req);
		}
	}
	if (req.getMethod() == "DELETE")
	{
		// Convert the request path to a relative path
    	std::string relativePath = req.getPath();
    	if (!relativePath.empty() && relativePath[0] == '/')
		{
        	relativePath = relativePath.substr(1);
    	}

		if (relativePath.find("uploads/") != 0)
		{
			customError = getCustomErrorPage(filepath, route, 403, server);
			return _serveFile(customError, 403, req);
		}

		std::string fullFilePath = "./" + relativePath;

		// Check if the uploads directory is writable
		if (!util::directory_is_writable("./uploads"))
		{
			customError = getCustomErrorPage(filepath, route, 403, server);
			return _serveFile(customError, 403, req);
		}
		// Check if the file exists and delete it
		if (util::fileExists(fullFilePath))
		{
			if (remove(fullFilePath.c_str()) == 0)
			{
				return _serveFile(filepath, 200, req);
			}
			else
			{
				customError = getCustomErrorPage(filepath, route, 500, server);
				return _serveFile(customError, 500, req);
			}
		}
		else
		{
			customError = getCustomErrorPage(filepath, route, 404, server);
			return _serveFile(customError, 404, req);
		}
	}
	if (!(req.getMethod() == "POST" && req.getPath() == "/fileupload"))
	{
		if (!route.is_readable(filepath))
		{
			valid = false;
			customError = getCustomErrorPage(filepath, route, 403, server);
			return _serveFile(customError, 403, req);
		}
	}
	customError = getCustomErrorPage(filepath, route, 404, server);
	return _serveFile(customError, 404, req);
}

std::string getContentType(const std::string &filepath)
{
	if (util::ends_with(filepath, ".html") || util::ends_with(filepath, ".htm"))
		return "text/html";
	else if (util::ends_with(filepath, ".txt"))
		return "text/plain";
	else if (util::ends_with(filepath, ".css"))
		return "text/css";
	else if (util::ends_with(filepath, ".js"))
		return "application/javascript";
	else if (util::ends_with(filepath, ".json"))
		return "application/json";
	else if (util::ends_with(filepath, ".jpg") || util::ends_with(filepath, ".jpeg"))
		return "image/jpeg";
	else if (util::ends_with(filepath, ".png"))
		return "image/png";
	else if (util::ends_with(filepath, ".gif"))
		return "image/gif";
	else if (util::ends_with(filepath, ".svg"))
		return "image/svg+xml";
	else if (util::ends_with(filepath, ".ico"))
		return "image/x-icon";
	else if (util::ends_with(filepath, ".pdf"))
		return "application/pdf";
	else if (util::ends_with(filepath, ".zip"))
		return "application/zip";
	else if (util::ends_with(filepath, ".mp3"))
		return "audio/mpeg";
	else if (util::ends_with(filepath, ".mp4"))
		return "video/mp4";
	else
		return "application/octet-stream"; // Default for unknown file types
}

std::string RequestRouter::_serveFile(const std::string &contentOrFilepath, int statusCode, Request &req)
{
    std::string content = "";
	bool isRawHtml = false;

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
			isRawHtml = true;

        }
    }
    std::string statusLine;
    switch (statusCode)
	{
		case 200:
			statusLine = "HTTP/1.1 200 OK";
			break;
		case 301:
			statusLine = "HTTP/1.1 301 Moved Permanently";
			break;
		case 302:
			statusLine = "HTTP/1.1 302 Found";
			break;
		case 303:
			statusLine = "HTTP/1.1 303 See Other";
			break;
		case 307:
			statusLine = "HTTP/1.1 307 Temporary Redirect";
			break;
		case 308:
			statusLine = "HTTP/1.1 308 Permanent Redirect";
			break;
		case 400:
			statusLine = "HTTP/1.1 400 Bad Request";
			break;
		case 403:
			statusLine = "HTTP/1.1 403 Forbidden";
			break;
		case 404:
			statusLine = "HTTP/1.1 404 Not Found";
			break;
		case 405:
			statusLine = "HTTP/1.1 405 Method Not Allowed";
			break;
		case 413:
			statusLine = "HTTP/1.1 413 Payload Too Large";
			break;
		case 500:
			statusLine = "HTTP/1.1 500 Internal Server Error";
			break;
		case 502:
			statusLine = "HTTP/1.1 502 Bad Gateway";
			break;
		case 503:
			statusLine = "HTTP/1.1 503 Service Unavailable";
			break;
		case 504:
			statusLine = "HTTP/1.1 504 Gateway Timeout";
			break;
		default:
			statusLine = "HTTP/1.1 500 Internal Server Error";
	}
	if (isRawHtml)
	{
		statusLine += "\r\nContent-Type: text/html";
	}
	else
	{
		statusLine += "\r\nContent-Type: " + getContentType(contentOrFilepath);
	}
    //statusLine += "\r\nContent-Type: text/html";
	//statusLine += "\r\nContent-Type: image/png";

    statusLine += "\r\nContent-Length: ";
    statusLine += util::to_string(content.size());

	bool isRedirect = (statusCode >= 300 && statusCode < 400);
	if (isRedirect)
	{
		std::string locationPath = contentOrFilepath;
		if (locationPath.find("http://") != 0 && locationPath.find("https://") != 0 && locationPath[0] != '/')
		{
			locationPath = "/" + locationPath;
		}

		statusLine += "\r\nLocation: " + locationPath;
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

    if (!(statusCode >= 300 && statusCode < 400))
	{
		statusLine += content;
	}

	//std::cout << "response: " << statusLine << std::endl;
    return statusLine;
}



Route RequestRouter::_getRoute(const Server &server, Request &req)
{

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

    if (bestMatch)
    {
        return *bestMatch;
    }
    return Route();
}

