#include "Uploader.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Request.hpp"

std::string FileUploader::_extractBoundary(const std::string &rawRequest)
{
    size_t pos = rawRequest.find("boundary=");
    if (pos == std::string::npos)
        return "";
    return "--" + rawRequest.substr(pos + 9, rawRequest.find("\r\n", pos) - (pos + 9));
}

std::string FileUploader::_extractFilename(const std::string &rawRequest)
{
    size_t pos = rawRequest.find("Content-Disposition: form-data; name=\"file\"; filename=\"");
    if (pos == std::string::npos)
        return "unknown"; // Default if no filename found

    size_t start = pos + 55; // Length of the search string above
    size_t end = rawRequest.find("\"", start);
    return rawRequest.substr(start, end - start);
}

std::string FileUploader::_extractFileContent(const std::string &rawRequest, const std::string &boundary_param)
{
    size_t start = rawRequest.find("\r\n\r\n", rawRequest.find(boundary_param)) + 4; // Start of file content
    size_t end = rawRequest.find(boundary_param, start) - 2;                          // End of file content (before next boundary)
    return rawRequest.substr(start, end - start);
}

bool FileUploader::handleRequest(const std::string &rawRequest)
{
    _boundary = _extractBoundary(rawRequest);
    _filename = _extractFilename(rawRequest);
    _fileContent = _extractFileContent(rawRequest, _boundary);

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
