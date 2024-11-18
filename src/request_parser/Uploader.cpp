#include "Uploader.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Request.hpp"

FileUploader::FileUploader(const std::string &body)
{
    _boundary = _extractBoundary(body);
    _filename = _extractFilename(body);
    _fileContent = _extractFileContent(body, _boundary);
}


std::string FileUploader::_extractBoundary(const std::string &body)
{
    size_t pos = body.find("Boundary");
    if (pos == std::string::npos)
        return "";

    size_t start = pos + 8; // Move past "Boundary"
    size_t end = body.find("\r\n", start);
    return body.substr(start, end - start);
}

std::string FileUploader::_extractFilename(const std::string &body)
{	
    size_t pos = body.find("Content-Disposition: form-data;");
    if (pos == std::string::npos)
        return "unknown"; 

    size_t filename_pos = body.find("filename=\"", pos);
    if (filename_pos == std::string::npos)
        return "unknown";

    size_t start = filename_pos + 10; // "filename=\"" length
    size_t end = body.find("\"", start);
    if (end == std::string::npos)
        return "unknown";

    return body.substr(start, end - start);
}

std::string FileUploader::_extractFileContent(const std::string &body, const std::string &boundary_param)
{
    size_t content_type_pos = body.find("Content-Type:");
    if (content_type_pos == std::string::npos)
    {
        std::cerr << "Content-Type not found!\n";
        return "";
    }

    size_t content_start = body.find("\n", content_type_pos) + 1; 
    if (content_start == std::string::npos)
    {
        std::cerr << "Start of content not found!\n";
        return "";
    }

    // Skip the empty line after headers
    content_start = body.find("\n", content_start) + 1;

    // Find the next boundary
    size_t content_end = body.find("\n------WebKitFormBoundary" + boundary_param, content_start);
    if (content_end == std::string::npos)
    {
        std::cerr << "End boundary not found!\n";
        return "";
    }

	if (content_end > 0 && body[content_end - 1] == '\r')
    {
        content_end -= 1;
    }
    return body.substr(content_start, content_end - content_start);
}



bool FileUploader::handleRequest()
{
	if (_filename == "unknown")
    {
        return false;
    }
    mkdir("uploads", 0777);

	std::ofstream file(("uploads/" + _filename).c_str(), std::ios::binary);
    if (file)
    {
        file << _fileContent;
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool FileUploader::isMalformed() const
{
    return _boundary.empty() || _filename == "unknown";
}