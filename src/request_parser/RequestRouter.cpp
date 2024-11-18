#include "RequestRouter.hpp"
#include "Uploader.hpp"
#include "util.hpp"
#include "webserv.hpp"

#include <fstream>     // For std::ifstream
#include <string>

std::string RequestRouter::route(const Request &req)
{
    if (!req.isValid()) // Early exit for invalid requests
    {
        return _serveFile("root/400.html", 400);
    }

    if (req.getMethod() == "POST" && req.getPath() == "/upload")
    {
        int contentLength = 0;
        std::istringstream iss(req.getHeader("Content-Length"));
        if (req.getHeader("Content-Length").empty() || !(iss >> contentLength) || contentLength > MAX_UPLOAD_SIZE)
        {
            return _serveFile("root/413.html", 413); // Payload too large
        }

        FileUploader uploader(req.getRawRequest());
        if (uploader.isMalformed())
        {
            return _serveFile("root/400.html", 400); // Malformed request
        }

        if (uploader.handleRequest())
        {
            return "HTTP/1.1 303 See Other\r\n"
                   "Location: /303.html\r\n"
                   "Connection: close\r\n"
                   "\r\n";
        }

        return _serveFile("root/500.html", 500); // Internal server error
    }

    if (req.getMethod() == "GET")
    {
        if (req.getPath() == "/")
        {
            return _serveFile("root/index.html");
        }
        return _serveFile("root" + req.getPath());
    }
    return _serveFile("root/404.html", 404);
}

std::string RequestRouter::_serveFile(const std::string &filepath, int statusCode)
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
    {
        return _serveFile("root/404.html", 404);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();

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
    return statusLine + "\r\nContent-Type: text/html\r\nContent-Length: " + util::to_string(content.size()) + "\r\n\r\n" + content;
}
