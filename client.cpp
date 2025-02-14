#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <chrono>
#include <iostream>

using namespace std;

int main() {
    struct sockaddr_rc addr = { 0 };
    int s, status;
    // Replace the following with the server's Bluetooth address
    char dest[18] = "D0:04:B0:2B:93:28"; 

    // Create a Bluetooth socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set connection parameters (server's address and channel)
    addr.rc_family = AF_BLUETOOTH;
    str2ba(dest, &addr.rc_bdaddr);
    addr.rc_channel = (uint8_t) 1;

    // Connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (status < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    cout << "Client: Connected to server " << dest << endl;

    const int iterations = 10;
    for (int i = 0; i < iterations; i++) {
        const char* msg = "ping";
        char buf[1024] = { 0 };

        // Record the time before sending the message
        auto start = chrono::steady_clock::now();

        // Send the ping message
        if (write(s, msg, strlen(msg)) < 0) {
            perror("write");
            break;
        }

        // Wait for the echoed reply from the server
        if (read(s, buf, sizeof(buf)) < 0) {
            perror("read");
            break;
        }

        // Record the time after receiving the reply
        auto end = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();

        cout << "Client: Iteration " << (i + 1)
             << " - Round Trip Time = " << elapsed << " microseconds" << endl;
        sleep(1); // Pause for a second between pings
    }

    close(s);
    return 0;
}
