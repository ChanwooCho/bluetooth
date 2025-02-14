#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>

int main() {
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr);

    // Create a Bluetooth socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind socket to the local Bluetooth adapter and channel 1
    loc_addr.rc_family = AF_BLUETOOTH;
    // Using str2ba to set address to any:
    str2ba("00:00:00:00:00:00", &loc_addr.rc_bdaddr);
    loc_addr.rc_channel = (uint8_t) 1;
    if (bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(s, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server: Waiting for connection...\n");

    // Accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    if (client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char client_addr[18];
    ba2str(&rem_addr.rc_bdaddr, client_addr);
    printf("Server: Accepted connection from %s\n", client_addr);

    // Echo loop: read data and immediately send it back
    while (true) {
        bytes_read = read(client, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("Server: Received [%s]\n", buf);
            if (write(client, buf, bytes_read) <= 0) {
                perror("write");
                break;
            }
        } else {
            if (bytes_read == 0) {
                printf("Server: Client disconnected\n");
            } else {
                perror("read");
            }
            break;
        }
        // Clear the buffer for the next message
        memset(buf, 0, sizeof(buf));
    }

    close(client);
    close(s);
    return 0;
}
