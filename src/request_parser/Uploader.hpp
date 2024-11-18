#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <string>

class FileUploader
{
    private:
        std::string _boundary;
        std::string _filename;
        std::string _fileContent;

        std::string _extractBoundary(const std::string &raw_request);
        std::string _extractFilename(const std::string &raw_request);
        std::string _extractFileContent(const std::string &raw_request, const std::string &boundary);

    public:
		FileUploader(const std::string &raw_request);
        bool handleRequest();
		bool isMalformed() const;
		~FileUploader() {};
};

#endif
