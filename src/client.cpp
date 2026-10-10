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

    // Send SETNX command
    const char* setnxMessage = "*3\r\n$5\r\nSETNX\r\n$8\r\nusername\r\n$4\r\nJeet\r\n";
    result = send(
        clientSocket,
        setnxMessage,
        static_cast<int>(strlen(setnxMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "SETNX send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "SETNX command sent\n";
    char setnxBuffer[1024]{};
    result = recv(
        clientSocket,
        setnxBuffer,
        sizeof(setnxBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "SETNX receive failed: " << WSAGetLastError() << "\n";
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
    setnxBuffer[result] = '\0';
    std::cout << "SETNX response: " << setnxBuffer << "\n";

    // Send SETNX command again
    const char* secondSetnxMessage = "*3\r\n$5\r\nSETNX\r\n$8\r\nusername\r\n$5\r\nRahul\r\n";
    result = send(
        clientSocket,
        secondSetnxMessage,
        static_cast<int>(strlen(secondSetnxMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "Second SETNX send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Second SETNX command sent\n";
    char secondSetnxBuffer[1024]{};
    result = recv(
        clientSocket,
        secondSetnxBuffer,
        sizeof(secondSetnxBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "Second SETNX receive failed: " << WSAGetLastError() << "\n";
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
    secondSetnxBuffer[result] = '\0';
    std::cout << "Second SETNX response: " << secondSetnxBuffer << "\n";

    // Send APPEND command
    const char* appendMessage = "*3\r\n$6\r\nAPPEND\r\n$8\r\nusername\r\n$9\r\nSwarnakar\r\n";
    result = send(
        clientSocket,
        appendMessage,
        static_cast<int>(strlen(appendMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "APPEND send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "APPEND command sent\n";
    char appendBuffer[1024]{};
    result = recv(
        clientSocket,
        appendBuffer,
        sizeof(appendBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "APPEND receive failed: " << WSAGetLastError() << "\n";
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
    appendBuffer[result] = '\0';
    std::cout << "APPEND response: " << appendBuffer << "\n";

    // Send STRLEN command
    const char* strlenMessage = "*2\r\n$6\r\nSTRLEN\r\n$8\r\nusername\r\n";
    result = send(
        clientSocket,
        strlenMessage,
        static_cast<int>(strlen(strlenMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "STRLEN send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "STRLEN command sent\n";
    char strlenBuffer[1024]{};
    result = recv(
        clientSocket,
        strlenBuffer,
        sizeof(strlenBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "STRLEN receive failed: " << WSAGetLastError() << "\n";
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
    strlenBuffer[result] = '\0';
    std::cout << "STRLEN response: " << strlenBuffer << "\n";

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

    // Send PTTL command
    const char* pttlMessage = "*2\r\n$4\r\nPTTL\r\n$4\r\nname\r\n";
    result = send(
        clientSocket,
        pttlMessage,
        static_cast<int>(strlen(pttlMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "PTTL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PTTL command sent\n";
    char pttlBuffer[1024]{};
    result = recv(
        clientSocket,
        pttlBuffer,
        sizeof(pttlBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "PTTL receive failed: " << WSAGetLastError() << "\n";
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
    pttlBuffer[result] = '\0';
    std::cout << "PTTL response: " << pttlBuffer << "\n";

    // Send PERSIST test SET command
    const char* persistSetMessage = "*3\r\n$3\r\nSET\r\n$7\r\npersist\r\n$4\r\nJeet\r\n";
    result = send(clientSocket, persistSetMessage, static_cast<int>(strlen(persistSetMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST test SET send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PERSIST test SET command sent\n";
    char persistSetBuffer[1024]{};
    result = recv(clientSocket, persistSetBuffer, sizeof(persistSetBuffer) - 1, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST test SET receive failed: " << WSAGetLastError() << "\n";
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
    persistSetBuffer[result] = '\0';
    std::cout << "PERSIST test SET response: " << persistSetBuffer << "\n";

    // Send EXPIRE command for PERSIST test
    const char* persistExpireMessage = "*3\r\n$6\r\nEXPIRE\r\n$7\r\npersist\r\n$1\r\n3\r\n";
    result = send(clientSocket, persistExpireMessage, static_cast<int>(strlen(persistExpireMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST test EXPIRE send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PERSIST test EXPIRE command sent\n";
    char persistExpireBuffer[1024]{};
    result = recv(clientSocket, persistExpireBuffer, sizeof(persistExpireBuffer) - 1, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST test EXPIRE receive failed: " << WSAGetLastError() << "\n";
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
    persistExpireBuffer[result] = '\0';
    std::cout << "PERSIST test EXPIRE response: " << persistExpireBuffer << "\n";

    // Send PERSIST command
    const char* persistMessage = "*2\r\n$7\r\nPERSIST\r\n$7\r\npersist\r\n";
    result = send(clientSocket, persistMessage, static_cast<int>(strlen(persistMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PERSIST command sent\n";
    char persistBuffer[1024]{};
    result = recv(clientSocket, persistBuffer, sizeof(persistBuffer) - 1, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST receive failed: " << WSAGetLastError() << "\n";
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
    persistBuffer[result] = '\0';
    std::cout << "PERSIST response: " << persistBuffer << "\n";

    // Check TTL after PERSIST
    const char* persistTtlMessage = "*2\r\n$3\r\nTTL\r\n$7\r\npersist\r\n";
    result = send(clientSocket, persistTtlMessage, static_cast<int>(strlen(persistTtlMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST TTL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PERSIST TTL command sent\n";
    char persistTtlBuffer[1024]{};
    result = recv(clientSocket, persistTtlBuffer, sizeof(persistTtlBuffer) - 1, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST TTL receive failed: " << WSAGetLastError() << "\n";
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
    persistTtlBuffer[result] = '\0';
    std::cout << "PERSIST TTL response: " << persistTtlBuffer << "\n";

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

    // Send expired PTTL command
    const char* expiredPttlMessage = "*2\r\n$4\r\nPTTL\r\n$4\r\nname\r\n";
    result = send(
        clientSocket,
        expiredPttlMessage,
        static_cast<int>(strlen(expiredPttlMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "Expired PTTL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Expired PTTL command sent\n";
    char expiredPttlBuffer[1024]{};
    result = recv(
        clientSocket,
        expiredPttlBuffer,
        sizeof(expiredPttlBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "Expired PTTL receive failed: " << WSAGetLastError() << "\n";
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
    expiredPttlBuffer[result] = '\0';
    std::cout << "Expired PTTL response: " << expiredPttlBuffer << "\n";

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

    // Check PERSIST key after the original expiration time
    const char* persistGetMessage = "*2\r\n$3\r\nGET\r\n$7\r\npersist\r\n";
    result = send(clientSocket, persistGetMessage, static_cast<int>(strlen(persistGetMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST GET send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "PERSIST GET command sent\n";
    char persistGetBuffer[1024]{};
    result = recv(clientSocket, persistGetBuffer, sizeof(persistGetBuffer) - 1, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "PERSIST GET receive failed: " << WSAGetLastError() << "\n";
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
    persistGetBuffer[result] = '\0';
    std::cout << "PERSIST GET response: " << persistGetBuffer << "\n";

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

    // Send MSET command
    const char* msetMessage = "*5\r\n$4\r\nMSET\r\n$4\r\nname\r\n$4\r\nJeet\r\n$4\r\ncity\r\n$9\r\nBangalore\r\n";
    result = send(
        clientSocket,
        msetMessage,
        static_cast<int>(strlen(msetMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "MSET send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "MSET command sent\n";
    char msetBuffer[1024]{};
    result = recv(
        clientSocket,
        msetBuffer,
        sizeof(msetBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "MSET receive failed: " << WSAGetLastError() << "\n";
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
    msetBuffer[result] = '\0';
    std::cout << "MSET response: " << msetBuffer << "\n";

    // Send MGET command
    const char* mgetMessage = "*4\r\n$4\r\nMGET\r\n$4\r\nname\r\n$4\r\ncity\r\n$7\r\nmissing\r\n";
    result = send(
        clientSocket,
        mgetMessage,
        static_cast<int>(strlen(mgetMessage)),
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "MGET send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "MGET command sent\n";
    char mgetBuffer[1024]{};
    result = recv(
        clientSocket,
        mgetBuffer,
        sizeof(mgetBuffer) - 1,
        0
    );
    if (result == SOCKET_ERROR) {
        std::cerr << "MGET receive failed: " << WSAGetLastError() << "\n";
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
    mgetBuffer[result] = '\0';
    std::cout << "MGET response: " << mgetBuffer << "\n";

    
    // Step 50: Test successful deletion of an existing key
    const char* deleteSetMessage = "*3\r\n$3\r\nSET\r\n$10\r\ndeleteTest\r\n$4\r\nJeet\r\n";
    result = send(clientSocket, deleteSetMessage, static_cast<int>(strlen(deleteSetMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "SET deleteTest send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "SET deleteTest command sent\n";
    char deleteSetBuffer[1024]{};
    result = recv(clientSocket, deleteSetBuffer, sizeof(deleteSetBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "SET deleteTest receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteSetBuffer[result] = '\0';
    std::cout << "SET deleteTest response: " << deleteSetBuffer << "\n";

    // Step 50: Delete the existing key
    const char* deleteMessage = "*2\r\n$3\r\nDEL\r\n$10\r\ndeleteTest\r\n";
    result = send(clientSocket, deleteMessage, static_cast<int>(strlen(deleteMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "First DEL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "First DEL command sent\n";
    char deleteBuffer[1024]{};
    result = recv(clientSocket, deleteBuffer, sizeof(deleteBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "First DEL receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteBuffer[result] = '\0';
    std::cout << "First DEL response: " << deleteBuffer << "\n";

    // Step 50: Verify deleting the same key again
    result = send(clientSocket, deleteMessage, static_cast<int>(strlen(deleteMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Second DEL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Second DEL command sent\n";
    char secondDeleteBuffer[1024]{};
    result = recv(clientSocket, secondDeleteBuffer, sizeof(secondDeleteBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "Second DEL receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    secondDeleteBuffer[result] = '\0';
    std::cout << "Second DEL response: " << secondDeleteBuffer << "\n";

    // Step 50: Verify the deleted key has no remaining value
    const char* deleteGetMessage = "*2\r\n$3\r\nGET\r\n$10\r\ndeleteTest\r\n";
    result = send(clientSocket, deleteGetMessage, static_cast<int>(strlen(deleteGetMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "GET after DEL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "GET after DEL command sent\n";
    char deleteGetBuffer[1024]{};
    result = recv(clientSocket, deleteGetBuffer, sizeof(deleteGetBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "GET after DEL receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteGetBuffer[result] = '\0';
    std::cout << "GET after DEL response: " << deleteGetBuffer << "\n";

    // Verify deleting a key also removes its expiration metadata
    const char* deleteExpireSetMessage = "*3\r\n$3\r\nSET\r\n$10\r\ndeleteTest\r\n$4\r\nJeet\r\n";
    result = send(clientSocket, deleteExpireSetMessage, static_cast<int>(strlen(deleteExpireSetMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "TTL test SET send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "TTL test SET command sent\n";
    char deleteExpireSetBuffer[1024]{};
    result = recv(clientSocket, deleteExpireSetBuffer, sizeof(deleteExpireSetBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "TTL test SET receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteExpireSetBuffer[result] = '\0';
    std::cout << "TTL test SET response: " << deleteExpireSetBuffer << "\n";

    const char* deleteExpireMessage = "*3\r\n$6\r\nEXPIRE\r\n$10\r\ndeleteTest\r\n$2\r\n10\r\n";
    result = send(clientSocket, deleteExpireMessage, static_cast<int>(strlen(deleteExpireMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "EXPIRE test send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "EXPIRE test command sent\n";
    char deleteExpireBuffer[1024]{};
    result = recv(clientSocket, deleteExpireBuffer, sizeof(deleteExpireBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "EXPIRE test receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteExpireBuffer[result] = '\0';
    std::cout << "EXPIRE test response: " << deleteExpireBuffer << "\n";

    result = send(clientSocket, deleteMessage, static_cast<int>(strlen(deleteMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "DEL with TTL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "DEL with TTL command sent\n";
    char deleteTtlBuffer[1024]{};
    result = recv(clientSocket, deleteTtlBuffer, sizeof(deleteTtlBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "DEL with TTL receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteTtlBuffer[result] = '\0';
    std::cout << "DEL with TTL response: " << deleteTtlBuffer << "\n";

    const char* deleteTtlCheckMessage = "*2\r\n$3\r\nTTL\r\n$10\r\ndeleteTest\r\n";
    result = send(clientSocket, deleteTtlCheckMessage, static_cast<int>(strlen(deleteTtlCheckMessage)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "TTL after DEL send failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "TTL after DEL command sent\n";
    char deleteTtlCheckBuffer[1024]{};
    result = recv(clientSocket, deleteTtlCheckBuffer, sizeof(deleteTtlCheckBuffer) - 1, 0);
    if (result == SOCKET_ERROR || result == 0) {
        std::cerr << "TTL after DEL receive failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    deleteTtlCheckBuffer[result] = '\0';
    std::cout << "TTL after DEL response: " << deleteTtlCheckBuffer << "\n";


    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}