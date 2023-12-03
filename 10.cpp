// SimpleWebServer.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;

// Function to read the content of a file into a string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

    if (file) {
        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();
        return contents.str();
    }

    return "";
}

// Function to handle incoming HTTP requests
std::string handleHttpRequest(const std::string& request) {
    // For simplicity, always return the same HTML content
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n\r\n";
    response += "<html><body><h1>Welcome to My Simple Web Server!</h1></body></html>\r\n";
    return response;
}

int main() {
    // Step 1: Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Check for errors
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    // Step 2: Bind the socket to an IP address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080); // Port 8080

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket." << std::endl;
        close(serverSocket);
        return -1;
    }

    // Step 3: Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening for connections." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        // Step 4: Accept a connection
        sockaddr_in clientAddress;
        socklen_t clientAddrSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrSize);

        // Check for errors
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            close(serverSocket);
            return -1;
        }

        std::cout << "Connection accepted. Waiting for data..." << std::endl;

        // Step 5: Receive data from the client
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        // Check for errors or closed connection
        if (bytesRead <= 0) {
            std::cerr << "Error receiving data or connection closed." << std::endl;
            close(clientSocket);
            continue;
        }

        // Step 6: Handle the HTTP request
        std::string request(buffer, bytesRead);
        std::string response = handleHttpRequest(request);

        // Step 7: Send the HTTP response to the client
        ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);

        // Check for errors or incomplete send
        if (bytesSent != static_cast<ssize_t>(response.length())) {
            std::cerr << "Error sending data." << std::endl;
        }

        // Step 8: Close the client socket
        close(clientSocket);
    }

    // Step 9: Close the server socket (this part is not reached in this example)
    close(serverSocket);

    return 0;
}