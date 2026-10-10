#include <mutex>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")
// Store Redis key-value pairs
std::unordered_map<std::string, std::string> database;
// Store expiration time for keys in milliseconds
std::unordered_map<std::string, long long> expiration;

// Protect shared database and expiration maps
std::mutex databaseMutex;
// Control the active expiration worker
std::atomic<bool> stopExpirationWorker{false};

// Check whether a key has expired
bool isKeyExpired(const std::string& key) {
    auto iterator = expiration.find(key);

    if (iterator == expiration.end()) {
        return false;
    }

    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    if (currentTime >= iterator->second) {
        database.erase(key);
        expiration.erase(iterator);

        return true;
    }

    return false;
}

// Delete a key and its expiration metadata
bool deleteKey(const std::string& key) {
    auto iterator = database.find(key);
    if (iterator == database.end()) {
        return false;
    }

    database.erase(iterator);
    expiration.erase(key);

    return true;
}

// Active expiration worker
void activeExpirationWorker() {
    while (!stopExpirationWorker) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(databaseMutex);
        auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        auto iterator = expiration.begin();
        while (iterator != expiration.end()) {
            if (currentTime >= iterator->second) {
                database.erase(iterator->first);
                iterator = expiration.erase(iterator);
            } else {
                ++iterator;
            }
        }
    }
}

// Check whether a command operates on a key
bool commandUsesKey(const std::string& command) {
    return command == "SET" ||
           command == "SETNX" ||
           command == "APPEND" ||
           command == "STRLEN" ||
           command == "TYPE" ||
           command == "GET" ||
           command == "DEL" ||
           command == "EXISTS" ||
           command == "INCR" ||
           command == "DECR" ||
           command == "INCRBY" ||
           command == "DECRBY" ||
           command == "EXPIRE" ||
           command == "TTL" ||
           command == "PTTL" ||
           command == "PERSIST";
}

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

// Create a RESP array response
std::string encodeArray(const std::vector<std::string>& values) {
    std::string response = "*" + std::to_string(values.size()) + "\r\n";
    for (const std::string& value : values) {
        if (value == "__NULL__") {
            response += encodeNullBulkString();
        } else {
            response += encodeBulkString(value);
        }
    }

    return response;
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
            
            // Protect shared database during command execution
            std::lock_guard<std::mutex> lock(databaseMutex);

            std::string command = parsedCommand[0];
            std::transform(command.begin(), command.end(), command.begin(), ::toupper);

            // Check expiration before handling key-based commands
            if (commandUsesKey(command) && parsedCommand.size() >= 2) {
                isKeyExpired(parsedCommand[1]);
            }

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
                expiration.erase(key);
                std::string response = encodeSimpleString("OK");
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "SET command handled successfully\n";

            // Handle the SETNX command
            } else if (command == "SETNX" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                const std::string& value = parsedCommand[2];
                auto iterator = database.find(key);

                int result = 0;
                if (iterator == database.end()) {
                    database[key] = value;
                    result = 1;
                }

                std::string response = encodeInteger(result);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "SETNX command handled successfully\n";

            // Handle the APPEND command
            } else if (command == "APPEND" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                const std::string& value = parsedCommand[2];
                database[key] += value;

                int newLength = static_cast<int>(database[key].size());

                std::string response = encodeInteger(newLength);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "APPEND command handled successfully\n";
                
            // Handle the STRLEN command
            } else if (command == "STRLEN" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);
                int length = 0;
                if (iterator != database.end()) {
                    length = static_cast<int>(iterator->second.size());
                }

                std::string response = encodeInteger(length);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "STRLEN command handled successfully\n";

            // Handle the TYPE command
            } else if (command == "TYPE" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);
                std::string response;
                if (iterator == database.end()) {
                    response = encodeSimpleString("none");
                } else {
                    response = encodeSimpleString("string");
                }

                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "TYPE command handled successfully\n";

            // Handle the MSET command
            } else if (command == "MSET" && parsedCommand.size() >= 3 && parsedCommand.size() % 2 == 1) {
                for (size_t i = 1; i < parsedCommand.size(); i += 2) {
                    const std::string& key = parsedCommand[i];
                    const std::string& value = parsedCommand[i + 1];
                    database[key] = value;
                }

                std::string response = encodeSimpleString("OK");
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "MSET command handled successfully\n";
                
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
            
            // Handle the MGET command
            } else if (command == "MGET" && parsedCommand.size() >= 2) {
                std::vector<std::string> values;

                for (size_t i = 1; i < parsedCommand.size(); i++) {
                    const std::string& key = parsedCommand[i];
                    isKeyExpired(key);
                    auto iterator = database.find(key);
                    if (iterator == database.end()) {
                        values.push_back("__NULL__");
                    } else {
                        values.push_back(iterator->second);
                    }
                }

                std::string response = encodeArray(values);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "MGET command handled successfully\n";

            // Handle the DEL command
            } else if (command == "DEL" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                int deletedCount = deleteKey(key) ? 1 : 0;
                std::string response = encodeInteger(deletedCount);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "DEL command handled successfully\n";

            // Handle the INCR command
            } else if (command == "INCR" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);
                int value = 0;

                if (iterator != database.end()) {
                    try {
                        value = std::stoi(iterator->second);
                    } catch (...) {
                        std::string response = encodeError("ERR value is not an integer or out of range");
                        if (!sendResponse(clientSocket, response)) {
                            break;
                        }

                        std::cout << "INCR failed: value is not an integer\n";
                        continue;
                    }
                }

                value++;
                database[key] = std::to_string(value);
                std::string response = encodeInteger(value);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "INCR command handled successfully\n";
            
            // Handle the DECR command
            } else if (command == "DECR" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);
                int value = 0;

                if (iterator != database.end()) {
                    try {
                        value = std::stoi(iterator->second);
                    } catch (...) {
                        std::string response = encodeError("ERR value is not an integer or out of range");

                        if (!sendResponse(clientSocket, response)) {
                            break;
                        }

                        std::cout << "DECR failed: value is not an integer\n";
                        continue;
                    }
                }

                value--;
                database[key] = std::to_string(value);
                std::string response = encodeInteger(value);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "DECR command handled successfully\n";

            // Handle the INCRBY command
            } else if (command == "INCRBY" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                int increment = 0;
                try {
                    increment = std::stoi(parsedCommand[2]);
                } catch (...) {
                    std::string response = encodeError("ERR value is not an integer or out of range");
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "INCRBY failed: increment is not an integer\n";
                    continue;
                }

                auto iterator = database.find(key);
                int value = 0;
                if (iterator != database.end()) {
                    try {
                        value = std::stoi(iterator->second);
                    } catch (...) {
                        std::string response = encodeError("ERR value is not an integer or out of range");
                        if (!sendResponse(clientSocket, response)) {
                            break;
                        }

                        std::cout << "INCRBY failed: value is not an integer\n";
                        continue;
                    }
                }

                value += increment;
                database[key] = std::to_string(value);
                std::string response = encodeInteger(value);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "INCRBY command handled successfully\n";

            // Handle the DECRBY command
            } else if (command == "DECRBY" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                int decrement = 0;

                try {
                    decrement = std::stoi(parsedCommand[2]);
                } catch (...) {
                    std::string response = encodeError("ERR value is not an integer or out of range");
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "DECRBY failed: decrement is not an integer\n";
                    continue;
                }

                auto iterator = database.find(key);
                int value = 0;

                if (iterator != database.end()) {
                    try {
                        value = std::stoi(iterator->second);
                    } catch (...) {
                        std::string response = encodeError("ERR value is not an integer or out of range");
                        if (!sendResponse(clientSocket, response)) {
                            break;
                        }

                        std::cout << "DECRBY failed: value is not an integer\n";
                        continue;
                    }
                }

                value -= decrement;
                database[key] = std::to_string(value);
                std::string response = encodeInteger(value);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "DECRBY command handled successfully\n";

            // Handle the EXPIRE command
            } else if (command == "EXPIRE" && parsedCommand.size() == 3) {
                const std::string& key = parsedCommand[1];
                int seconds = 0;

                try {
                    seconds = std::stoi(parsedCommand[2]);
                } catch (...) {
                    std::string response = encodeError("ERR value is not an integer or out of range");

                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "EXPIRE failed: seconds is not an integer\n";
                    continue;
                }

                auto iterator = database.find(key);
                int result = 0;
                if (iterator != database.end()) {
                    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();

                    expiration[key] = currentTime + (static_cast<long long>(seconds) * 1000);
                    result = 1;
                }
                std::string response = encodeInteger(result);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "EXPIRE command handled successfully\n";

            // Handle the TTL command
            } else if (command == "TTL" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto databaseIterator = database.find(key);
                if (databaseIterator == database.end()) {
                    std::string response = encodeInteger(-2);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "TTL command handled successfully\n";
                    continue;
                }

                auto expirationIterator = expiration.find(key);
                if (expirationIterator == expiration.end()) {
                    std::string response = encodeInteger(-1);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "TTL command handled successfully\n";
                    continue;
                }

                long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                long long remainingMilliseconds = expirationIterator->second - currentTime;
                long long remainingTime = remainingMilliseconds / 1000;
                if (remainingTime <= 0) {
                    isKeyExpired(key);
                    std::string response = encodeInteger(-2);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "TTL command handled successfully\n";
                    continue;
                }

                std::string response = encodeInteger(static_cast<int>(remainingTime));
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "TTL command handled successfully\n";

            // Handle the PTTL command
            } else if (command == "PTTL" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto databaseIterator = database.find(key);
                if (databaseIterator == database.end()) {
                    std::string response = encodeInteger(-2);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "PTTL command handled successfully\n";
                    continue;
                }

                auto expirationIterator = expiration.find(key);
                if (expirationIterator == expiration.end()) {
                    std::string response = encodeInteger(-1);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "PTTL command handled successfully\n";
                    continue;
                }

                long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                long long remainingTime = expirationIterator->second - currentTime;
                if (remainingTime <= 0) {
                    isKeyExpired(key);
                    std::string response = encodeInteger(-2);
                    if (!sendResponse(clientSocket, response)) {
                        break;
                    }

                    std::cout << "PTTL command handled successfully\n";
                    continue;
                }

                std::string response = encodeInteger(remainingTime);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "PTTL command handled successfully\n";

            // Handle the PERSIST command
            } else if (command == "PERSIST" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                auto iterator = database.find(key);
                int result = 0;
                if (iterator != database.end()) {
                    auto expirationIterator = expiration.find(key);

                    if (expirationIterator != expiration.end()) {
                        expiration.erase(expirationIterator);
                        result = 1;
                    }
                }

                std::string response = encodeInteger(result);
                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "PERSIST command handled successfully\n";

            // Handle the EXISTS command
            } else if (command == "EXISTS" && parsedCommand.size() == 2) {
                const std::string& key = parsedCommand[1];
                int exists = database.find(key) != database.end();
                std::string response = encodeInteger(exists);

                if (!sendResponse(clientSocket, response)) {
                    break;
                }

                std::cout << "EXISTS command handled successfully\n";

            // Handle unknown commands and incorrect arguments
            } else {
                std::string response;
                if (command == "PING" || command == "ECHO" || command == "SET" || command == "GET" || command == "DEL" || command == "EXISTS" || command == "INCR" || command == "DECR" || command == "INCRBY" || command == "DECRBY" || command == "EXPIRE" || command == "TTL" || command == "PTTL" || command == "MSET" || command == "MGET" || command == "PERSIST") {
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

    // Start active expiration worker
    std::thread expirationThread(activeExpirationWorker);

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

    // Stop active expiration worker
    stopExpirationWorker = true;
    expirationThread.join();

    // Cleanup
    closesocket(serverSocket);
    // closesocket(clientSocket);
    WSACleanup();

    return 0;
}