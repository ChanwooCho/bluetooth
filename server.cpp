#include <iostream>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>

#define PORT 1

int main() {
    struct sockaddr_rc localAddr = {0}, remoteAddr = {0};
    char buffer[256] = {0};
    int serverSock, clientSock;
    socklen_t opt = sizeof(remoteAddr);

    // Create Bluetooth socket
    serverSock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (serverSock == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Bind socket to a Bluetooth address
    localAddr.rc_family = AF_BLUETOOTH;
    localAddr.rc_bdaddr = *BDADDR_ANY;
    localAddr.rc_channel = PORT;
    
    if (bind(serverSock, (struct sockaddr*)&localAddr, sizeof(localAddr)) == -1) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    // Listen for connection
    listen(serverSock, 1);
    std::cout << "Waiting for connection...\n";

    // Accept a connection
    clientSock = accept(serverSock, (struct sockaddr*)&remoteAddr, &opt);
    if (clientSock == -1) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Client connected!\n";

    while (true) {
        int bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            send(clientSock, buffer, bytesReceived, 0); // Echo back
        }
    }

    close(clientSock);
    close(serverSock);
    return 0;
}
