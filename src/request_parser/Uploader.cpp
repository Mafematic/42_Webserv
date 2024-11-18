#include "Uploader.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Request.hpp"

FileUploader::FileUploader(const std::string &raw_request)
{
    _boundary = _extractBoundary(raw_request);
    _filename = _extractFilename(raw_request);
    _fileContent = _extractFileContent(raw_request, _boundary);
}


std::string FileUploader::_extractBoundary(const std::string &rawRequest)
{
    size_t pos = rawRequest.find("boundary=");
    if (pos == std::string::npos)
        return "";

    size_t start = pos + 9; // Move past "boundary="
    size_t end = rawRequest.find("\r\n", start);
    return rawRequest.substr(start, end - start); // No need for "--" prefix here.
}

std::string FileUploader::_extractFilename(const std::string &rawRequest)
{	
    size_t pos = rawRequest.find("Content-Disposition: form-data;");
    if (pos == std::string::npos)
        return "unknown"; 

    size_t filename_pos = rawRequest.find("filename=\"", pos);
    if (filename_pos == std::string::npos)
        return "unknown";

    size_t start = filename_pos + 10; // "filename=\"" length
    size_t end = rawRequest.find("\"", start);
    if (end == std::string::npos)
        return "unknown";

    return rawRequest.substr(start, end - start);
}


std::string FileUploader::_extractFileContent(const std::string &rawRequest, const std::string &boundary_param)
{
    size_t start = rawRequest.find("\r\n\r\n", rawRequest.find(boundary_param)) + 4; // Start of file content
    size_t end = rawRequest.find(boundary_param, start) - 2;                          // End of file content (before next boundary)
    return rawRequest.substr(start, end - start);
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