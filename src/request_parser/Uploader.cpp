#include "Uploader.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

FileUploader::FileUploader(const Request &req)
{
	_boundary = _extractBoundary(req);
	_filename = _extractFilename(req);
	_fileContent = _extractFileContent(req, _boundary);

}

std::string FileUploader::_extractBoundary(const Request &req)
{
	std::string contentType = req.getHeader("Content-Type");
	if (contentType.empty())
	{
		return "";
	}
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
	{
		return "";
	}
	return contentType.substr(boundaryPos + 9); 
}

std::string FileUploader::_extractFilename(const Request &req)
{
	const std::string &body = req.getBody();
	size_t pos = body.find("Content-Disposition: form-data;");
	if (pos == std::string::npos)
		return "unknown";

	size_t filename_pos = body.find("filename=\"", pos);
	if (filename_pos == std::string::npos)
		return "unknown";

	size_t start = filename_pos + 10; // Move past "filename=\""
	size_t end = body.find("\"", start);
	if (end == std::string::npos)
		return "unknown";

	return body.substr(start, end - start);
}

std::string FileUploader::_extractFileContent(const Request &req, const std::string &boundary_param)
{
	const std::string &body = req.getBody();

	size_t content_type_pos = body.find("Content-Type:");
	if (content_type_pos == std::string::npos)
	{
		std::cerr << "Content-Type not found!\n";
		return "";
	}

	// Locate the start of content
	size_t content_start = body.find("\n", content_type_pos) + 1;
	if (content_start == std::string::npos)
	{
		std::cerr << "Start of content not found!\n";
		return "";
	}
	// Skip the empty line
	content_start = body.find("\n", content_start) + 1;

	// Locate end of content
	std::string boundary = "\n--" + boundary_param;
	size_t content_end = body.find(boundary, content_start);
	if (content_end == std::string::npos)
	{
		std::cerr << "End boundary not found!\n";
		return "";
	}

	// Move back one line
	if (content_end > 0 && body[content_end - 1] == '\r')
	{
		content_end -= 1;
	}
	return body.substr(content_start, content_end - content_start);
}

bool FileUploader::handleRequest()
{
	if (get_filename() == "unknown")
	{
		return false;
	}
	if (get_file_content().empty())
    {
        return false;
    }
	mkdir("uploads", 0777);
	std::string uploadDir = "./uploads";
	if (access(uploadDir.c_str(), W_OK) != 0)
	{
		return false;
	}
	std::ofstream file(("uploads/" + get_filename()).c_str(), std::ios::binary);
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
	return _boundary.empty() || get_filename() == "unknown";
}

std::string FileUploader::get_filename() const
{
    return _filename;
}

std::string FileUploader::get_file_content() const
{
    return _fileContent;
}