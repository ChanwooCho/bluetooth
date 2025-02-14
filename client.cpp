#include <iostream>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>

#define PORT 1

int main() {
    struct sockaddr_rc serverAddr = {0};
    char buffer[256] = {0};
    int clientSock;

    // Create Bluetooth socket
    clientSock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (clientSock == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Set up server address
    serverAddr.rc_family = AF_BLUETOOTH;
    str2ba("D0:04:B0:2B:93:28", &serverAddr.rc_bdaddr); // Replace with server Bluetooth MAC
    serverAddr.rc_channel = PORT;

    // Connect to server
    if (connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Connected to server\n";

    while (true) {
        // Get current timestamp
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        snprintf(buffer, sizeof(buffer), "%ld.%09ld", start.tv_sec, start.tv_nsec);
        send(clientSock, buffer, strlen(buffer), 0);

        int bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            double rtt = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
            std::cout << "Round-trip time: " << rtt << " ms\n";
        }

        sleep(1); // Send every second
    }

    close(clientSock);
    return 0;
}
