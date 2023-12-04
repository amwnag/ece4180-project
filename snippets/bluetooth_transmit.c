#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <string.h>

#define SERVER_ADDRESS "B8:27:EB:12:D7:91"  // Replace with the Bluetooth address of your target device

int main() {
    struct sockaddr_rc addr = {0};
    int s, status;
    char message[] = "Hello, Bluetooth!";

    // Create a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the connection parameters
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;  // Use channel 1

    str2ba(SERVER_ADDRESS, &addr.rc_bdaddr);

    // Connect to the server
    status = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if (status == -1) {
        perror("Connection failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    // Send data
    status = write(s, message, sizeof(message));
    if (status == -1) {
        perror("Data transmission failed");
    } else {
        printf("Data sent successfully: %s\n", message);
        
    }

    // Close the socket
    close(s);

    return 0;
}
