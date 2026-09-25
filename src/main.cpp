#include <winsock2.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")
// Store Redis key-value pairs
std::unordered_map<std::string, std::string> database;

// Parse a RESP array command
std::vector<std::string> parseRESP(const std::string& request) {
    std::vector<std::string> command;
    if (request.empty() || request[0] != '*') {
        return command;
    }

    size_t position = request.find("\r\n");
    if (position == std::string::npos) {
        return command;
    }

    int argumentCount = std::stoi(request.substr(1, position - 1));
    position += 2;

    for (int i = 0; i < argumentCount; i++) {
        if (position >= request.size() || request[position] != '$') {
            command.clear();
            return command;
        }

        size_t lengthEnd = request.find("\r\n", position);
        if (lengthEnd == std::string::npos) {
            command.clear();
            return command;
        }

        int argumentLength = std::stoi(
            request.substr(position + 1, lengthEnd - position - 1)
        );

        position = lengthEnd + 2;
        if (position + argumentLength + 2 > request.size()) {
            command.clear();
            return command;
        }

        std::string argument = request.substr(position, argumentLength);
        command.push_back(argument);
        position += argumentLength + 2;
    }

    return command;
}

// Extract one complete RESP command from the buffer
bool extractRESPCommand(std::string& buffer, std::string& command) {
    if (buffer.empty() || buffer[0] != '*') {
        return false;
    }

    size_t position = 0;
    size_t lineEnd = buffer.find("\r\n", position);

    if (lineEnd == std::string::npos) {
        return false;
    }

    int argumentCount = std::stoi(buffer.substr(1, lineEnd - 1));
    position = lineEnd + 2;

    for (int i = 0; i < argumentCount; i++) {
        if (position >= buffer.size()) {
            return false;
        }

        if (buffer[position] != '$') {
            return false;
        }

        lineEnd = buffer.find("\r\n", position);

        if (lineEnd == std::string::npos) {
            return false;
        }

        int argumentLength = std::stoi(
            buffer.substr(position + 1, lineEnd - position - 1)
        );

        position = lineEnd + 2;
        if (buffer.size() < position + argumentLength + 2) {
            return false;
        }
        position += argumentLength;
        if (buffer.substr(position, 2) != "\r\n") {
            return false;
        }
        position += 2;
    }

    command = buffer.substr(0, position);
    buffer.erase(0, position);

    return true;
}

// Create a RESP simple string response
std::string encodeSimpleString(const std::string& message) {
    return "+" + message + "\r\n";
}

// Create a RESP error response
std::string encodeError(const std::string& message) {
    return "-" + message + "\r\n";
}

// Create a RESP bulk string response
std::string encodeBulkString(const std::string& message) {
    return "$" + std::to_string(message.size()) + "\r\n" + message + "\r\n";
}


// Create a RESP null bulk string response
std::string encodeNullBulkString() {
    return "$-1\r\n";
}

// Create a RESP integer response
std::string encodeInteger(int value) {
    return ":" + std::to_string(value) + "\r\n";
}

// Send the complete response to the client
bool sendResponse(SOCKET clientSocket, const std::string& response) {
    size_t totalSent = 0;

    while (totalSent < response.size()) {
        int result = send(
            clientSocket,
            response.c_str() + totalSent,
            static_cast<int>(response.size() - totalSent),
            0
        );
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << "\n";
            return false;
        }
        if (result == 0) {
            std::cerr << "Connection closed while sending\n";
            return false;
        }
        totalSent += result;
    }

    return true;
}

void handleClient(SOCKET clientSocket) {
    // Store TCP data until complete RESP commands are available
    std::string receiveBuffer;

    // Keep the client connection alive
    while (true) {
        // Receive raw TCP 
        char buffer[1024]{};
        int result = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (result == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << "\n";
            break;
        } else if (result == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        // Add received data to the persistent buffer
        receiveBuffer.append(buffer, result);
        std::string request;
        while (extractRESPCommand(receiveBuffer, request)) {
            std::cout << "Received complete RESP command: " << request << "\n";

            // Parse the complete RESP command
            std::vector<std::string> parsedCommand = parseRESP(request);

            if (parsedCommand.empty()) {
                std::string response = encodeError("ERR invalid RESP request");
                if (!sendResponse(clientSocket, response)) {
                    break;
                }
                if (result == SOCKET_ERROR) {
                    std::cerr << "Send failed: " << WSAGetLastError() << "\n";
                    break;
                }
                continue;
            }

            std::string command = parsedCommand[0];
            std::transform(command.begin(), command.end(), command.begin(), ::toupper);

            // Handle the PING command
            if (command == "PING" && parsedCommand.size() == 1) {
                std::string response = encodeSimpleString("PONG");
                if (!sendResponse(clientSocket, response)) {
                    break;
                }
                std::cout << "PING command handled successfully\n";

            // Handle the ECHO command
            } else if (command == "ECHO" && parsedCommand.size() == 2) {
                const std::string& argument = parsedCommand[1];
                std::string response = encodeBulkString(argument);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "ECHO command handled successfully\n";

            // Handle the SET command
            } else if (command == "SET" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                const std::string& value = parsedCommand[2];

                database[key] = value;
                std::string response = encodeSimpleString("OK");
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "SET command handled successfully\n";

            // Handle the GET command
            } else if (command == "GET" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);

                std::string response;
                if (iterator == database.end()) {
                    response = encodeNullBulkString();
                } else {
                    response = encodeBulkString(iterator->second);
                }

                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                if (result == SOCKET_ERROR) {
                    std::cerr << "Send failed: " << WSAGetLastError() << "\n";
                    break;
                }

                std::cout << "GET command handled successfully\n";
            
            // Handle the DEL command
            } else if (command == "DEL" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);

                int deletedCount = 0;
                if (iterator != database.end()) {
                    database.erase(iterator);
                    deletedCount = 1;
                }

                std::string response = encodeInteger(deletedCount);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "DEL command handled successfully\n";

            // Handle unknown commands and incorrect arguments
            } else {
                std::string response;
                if (command == "PING" || command == "ECHO" || command == "SET" || command == "GET" || command == "DEL") {
                    response = encodeError("ERR wrong number of arguments for command");
                } else {
                    response = encodeError("ERR unknown command");
                }

                if (!sendResponse(clientSocket, response)) {
                    break;
                }
                std::cout << "Error response sent\n";
            }
        }
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData{};

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    std::cout << "Winsock initialized\n";

    // Create a TCP socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";

        WSACleanup();
        return 1;
    }

    std::cout << "Socket created successfully\n";

    // Configure server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddress.sin_port = htons(6379);

    // Bind socket to address and port
    result = bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));

    if (result == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << "\n";

        closesocket(serverSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Socket bound successfully\n";
    std::cout << "Server address: 127.0.0.1:6379\n";

    // Listen for incoming connections
    result = listen(serverSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";

        closesocket(serverSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Server is listening...\n";

    // Accept clients continuously
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";

            break;
        }

        std::cout << "Client connected successfully\n";

        // Receive data from the client
        handleClient(clientSocket);

        closesocket(clientSocket);
    }

    // Cleanup
    closesocket(serverSocket);
    // closesocket(clientSocket);
    WSACleanup();

    return 0;
}