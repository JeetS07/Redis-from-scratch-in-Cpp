#include <winsock2.h>
#include <windows.h>
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

    // Send EXPIRE command
    const char* expireMessage = "*3\r\n$6\r\nEXPIRE\r\n$4\r\nname\r\n$1\r\n3\r\n";

    result = send(
        clientSocket,
        expireMessage,
        static_cast<int>(strlen(expireMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "EXPIRE send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "EXPIRE command sent\n";

    char expireBuffer[1024]{};

    result = recv(
        clientSocket,
        expireBuffer,
        sizeof(expireBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "EXPIRE receive failed: " << WSAGetLastError() << "\n";

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

    expireBuffer[result] = '\0';

    std::cout << "EXPIRE response: " << expireBuffer << "\n";

    // Send TTL command
    const char* ttlMessage = "*2\r\n$3\r\nTTL\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        ttlMessage,
        static_cast<int>(strlen(ttlMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "TTL send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "TTL command sent\n";

    char ttlBuffer[1024]{};

    result = recv(
        clientSocket,
        ttlBuffer,
        sizeof(ttlBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "TTL receive failed: " << WSAGetLastError() << "\n";

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

    ttlBuffer[result] = '\0';

    std::cout << "TTL response: " << ttlBuffer << "\n";

    // Wait for the key to expire
    std::cout << "Waiting for key to expire...\n";

    Sleep(4000);

    // Check TTL after expiration
    const char* expiredTtlMessage = "*2\r\n$3\r\nTTL\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        expiredTtlMessage,
        static_cast<int>(strlen(expiredTtlMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Expired TTL send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Expired TTL command sent\n";

    char expiredTtlBuffer[1024]{};

    result = recv(
        clientSocket,
        expiredTtlBuffer,
        sizeof(expiredTtlBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Expired TTL receive failed: " << WSAGetLastError() << "\n";

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

    expiredTtlBuffer[result] = '\0';

    std::cout << "Expired TTL response: " << expiredTtlBuffer << "\n";

    // Check GET after expiration
    const char* expiredGetMessage = "*2\r\n$3\r\nGET\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        expiredGetMessage,
        static_cast<int>(strlen(expiredGetMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Expired GET send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Expired GET command sent\n";

    char expiredGetBuffer[1024]{};

    result = recv(
        clientSocket,
        expiredGetBuffer,
        sizeof(expiredGetBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Expired GET receive failed: " << WSAGetLastError() << "\n";

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

    expiredGetBuffer[result] = '\0';

    std::cout << "Expired GET response: " << expiredGetBuffer << "\n";

    // Send INCR command
    const char* incrMessage = "*2\r\n$4\r\nINCR\r\n$7\r\ncounter\r\n";

    result = send(
        clientSocket,
        incrMessage,
        static_cast<int>(strlen(incrMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "INCR send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "INCR command sent\n";

    char incrBuffer[1024]{};

    result = recv(
        clientSocket,
        incrBuffer,
        sizeof(incrBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "INCR receive failed: " << WSAGetLastError() << "\n";

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

    incrBuffer[result] = '\0';

    std::cout << "INCR response: " << incrBuffer << "\n";

    // Send INCR command again
    result = send(
        clientSocket,
        incrMessage,
        static_cast<int>(strlen(incrMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Second INCR send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "Second INCR command sent\n";

    char secondIncrBuffer[1024]{};

    result = recv(
        clientSocket,
        secondIncrBuffer,
        sizeof(secondIncrBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "Second INCR receive failed: " << WSAGetLastError() << "\n";

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

    secondIncrBuffer[result] = '\0';

    std::cout << "Second INCR response: " << secondIncrBuffer << "\n";

    // Send INCRBY command
    const char* incrByMessage = "*3\r\n$6\r\nINCRBY\r\n$7\r\ncounter\r\n$2\r\n10\r\n";
    result = send(
        clientSocket,
        incrByMessage,
        static_cast<int>(strlen(incrByMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "INCRBY send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "INCRBY command sent\n";
    char incrByBuffer[1024]{};
    result = recv(
        clientSocket,
        incrByBuffer,
        sizeof(incrByBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "INCRBY receive failed: " << WSAGetLastError() << "\n";
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
    incrByBuffer[result] = '\0';
    std::cout << "INCRBY response: " << incrByBuffer << "\n";

    // Send DECRBY command
    const char* decrByMessage = "*3\r\n$6\r\nDECRBY\r\n$7\r\ncounter\r\n$1\r\n5\r\n";
    result = send(
        clientSocket,
        decrByMessage,
        static_cast<int>(strlen(decrByMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DECRBY send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "DECRBY command sent\n";
    char decrByBuffer[1024]{};

    result = recv(
        clientSocket,
        decrByBuffer,
        sizeof(decrByBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DECRBY receive failed: " << WSAGetLastError() << "\n";
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

    decrByBuffer[result] = '\0';
    std::cout << "DECRBY response: " << decrByBuffer << "\n";

    // Send DECR command
    const char* decrMessage = "*2\r\n$4\r\nDECR\r\n$7\r\ncounter\r\n";

    result = send(
        clientSocket,
        decrMessage,
        static_cast<int>(strlen(decrMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DECR send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "DECR command sent\n";
    char decrBuffer[1024]{};

    result = recv(
        clientSocket,
        decrBuffer,
        sizeof(decrBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "DECR receive failed: " << WSAGetLastError() << "\n";
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

    decrBuffer[result] = '\0';
    std::cout << "DECR response: " << decrBuffer << "\n";

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

    // Send EXISTS command
    const char* existsMessage = "*2\r\n$6\r\nEXISTS\r\n$4\r\nname\r\n";

    result = send(
        clientSocket,
        existsMessage,
        static_cast<int>(strlen(existsMessage)),
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "EXISTS send failed: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();

        return 1;
    }

    std::cout << "EXISTS command sent\n";

    char existsBuffer[1024]{};

    result = recv(
        clientSocket,
        existsBuffer,
        sizeof(existsBuffer) - 1,
        0
    );

    if (result == SOCKET_ERROR) {
        std::cerr << "EXISTS receive failed: " << WSAGetLastError() << "\n";

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

    existsBuffer[result] = '\0';

    std::cout << "EXISTS response: " << existsBuffer << "\n";

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