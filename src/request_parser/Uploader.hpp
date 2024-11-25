#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <string>
#include "Request.hpp"

class FileUploader
{
    private:
        std::string _boundary;
        std::string _filename;
        std::string _fileContent;

        std::string _extractBoundary(const Request &req);
        std::string _extractFilename(const Request &req);
        std::string _extractFileContent(const Request &req, const std::string &boundary);

    public:
		FileUploader(const Request &req);
        bool handleRequest();
		bool isMalformed() const;
		~FileUploader() {};
};

#endif
