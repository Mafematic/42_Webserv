#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // Server address setup
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000); // Change to your server port
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to your server IP

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    // Construct chunked HTTP request
    std::string request =
        "POST /path HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "4\r\n"
        "Wiki\r\n"
        "5\r\n"
        "pedia\r\n"
        "0\r\n"
        "\r\n";

    // Send request
    send(sockfd, request.c_str(), request.size(), 0);

    // Receive response
    char buffer[1024] = {0};
    int bytes_received = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes_received > 0) {
        std::cout << "Server response:\n" << buffer << std::endl;
    }

    // Close socket
    close(sockfd);
    return 0;
}
