
#include <winsock2.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

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
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
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

    // Connect to the server
    result = connect(
        clientSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Connected to Redis successfully\n";

    // Send UNKNOWN command
    const char* unknownMessage = "*1\r\n$7\r\nUNKNOWN\r\n";

    result = send(
        clientSocket,
        unknownMessage,
        static_cast<int>(strlen(unknownMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "UNKNOWN send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "UNKNOWN command sent\n";

    char unknownBuffer[1024]{};

    result = recv(
        clientSocket,
        unknownBuffer,
        sizeof(unknownBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "UNKNOWN receive failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    if (result == 0) {
        std::cout << "Redis disconnected\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    unknownBuffer[result] = '\0';

    std::cout << "UNKNOWN response: " << unknownBuffer << "\n";

    // Send SET command
    const char* setMessage = "*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$4\r\nJeet\r\n";

    result = send(
        clientSocket,
        setMessage,
        static_cast<int>(strlen(setMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "SET send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "SET command sent\n";

    char setBuffer[1024]{};

    result = recv(
        clientSocket,
        setBuffer,
        sizeof(setBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "SET receive failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    if (result == 0) {
        std::cout << "Redis disconnected\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    setBuffer[result] = '\0';

    std::cout << "SET response: " << setBuffer << "\n";

    // Send DEL command
    const char* delMessage = "*2\r\n$3\r\nDEL\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        delMessage,
        static_cast<int>(strlen(delMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DEL send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "DEL command sent\n";

    char delBuffer[1024]{};

    result = recv(
        clientSocket,
        delBuffer,
        sizeof(delBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DEL receive failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    if (result == 0) {
        std::cout << "Redis disconnected\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    delBuffer[result] = '\0';

    std::cout << "DEL response: " << delBuffer << "\n";

    // Send GET command
    const char* getMessage = "*2\r\n$3\r\nGET\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        getMessage,
        static_cast<int>(strlen(getMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "GET send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "GET command sent\n";

    char getBuffer[1024]{};

    result = recv(
        clientSocket,
        getBuffer,
        sizeof(getBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "GET receive failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    if (result == 0) {
        std::cout << "Redis disconnected\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    getBuffer[result] = '\0';

    std::cout << "GET response: " << getBuffer << "\n";

    // Send PING command
    const char* pingMessage = "*1\r\n$4\r\nPING\r\n";

    result = send(
        clientSocket,
        pingMessage,
        static_cast<int>(strlen(pingMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "PING send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "PING command sent\n";

    char pingBuffer[1024]{};

    result = recv(
        clientSocket,
        pingBuffer,
        sizeof(pingBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "PING receive failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    if (result == 0) {
        std::cout << "Redis disconnected\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    pingBuffer[result] = '\0';

    std::cout << "PING response: " << pingBuffer << "\n";

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}