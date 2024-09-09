#include <iostream>
#include <fstream>
#include <cstring>      // For memset
#include <cstdlib>      // For exit
#include <sys/types.h>  // For socket types
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For inet_pton and htons
#include <unistd.h>     // For close
#include <netinet/tcp.h> // For TCP_NODELAY
#include <sstream>      // For std::ostringstream

int main() {
    const char* server_ip = "127.0.0.1"; // Replace with the server IP address
    const int server_port = 8001;        // Replace with the server port
    const char* file_path = "/home/ychen2/Downloads/dog-puppy-on-garden-royalty-free-image-1586966191.avif";  // Replace with the path to your file
    // Open the file and read its contents
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << "\n";
        return EXIT_FAILURE;
    }

    std::string body_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket\n";
        exit(EXIT_FAILURE);
    }

    // Disable Nagle's algorithm to reduce delays
    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
        std::cerr << "Error setting TCP_NODELAY\n";
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port); // Convert port to network byte order

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported \n";
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Prepare the HTTP POST request
    std::cout << "Content-Length: " << body_content.size() << std::endl;
    std::ostringstream request;
    request << "POST /files/dog HTTP/1.1\r\n";
    request << "Host: " << server_ip << ":" << server_port << "\r\n";
    request << "Content-Type: application/x-www-form-urlencoded\r\n";
    request << "Content-Length: " << body_content.size() << "\r\n";
    request << "Connection: keep-alive\r\n"; // Keep the connection open
    request << "\r\n";
    request << body_content;

    std::string request_str = request.str();

    // Send the entire request to the server
    ssize_t total_sent = 0;
    ssize_t request_length = request_str.size();
    while (total_sent < request_length) {
        ssize_t bytes_sent = send(sockfd, request_str.c_str() + total_sent, request_length - total_sent, 0);
        if (bytes_sent < 0) {
            std::cerr << "Failed to send request\n";
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        total_sent += bytes_sent;
        std::cout << total_sent << std::endl;
    }

    std::cout << "POST request sent to the server.\n";

    // Optional: Read the server's response
    char buffer[1024];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Server response:\n" << buffer << std::endl;
    }

    // Close the socket
    close(sockfd);
    return 0;
}