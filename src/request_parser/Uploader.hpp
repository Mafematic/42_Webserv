#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <string>

class FileUploader
{
    private:
        std::string _boundary;
        std::string _filename;
        std::string _fileContent;

        std::string _extractBoundary(const std::string &body);
        std::string _extractFilename(const std::string &body);
        std::string _extractFileContent(const std::string &body, const std::string &boundary);

    public:
		FileUploader(const std::string &body);
        bool handleRequest();
		bool isMalformed() const;
		~FileUploader() {};
};

#endif
