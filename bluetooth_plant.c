#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <fcntl.h>
#include <string.h>

#define CHANNEL 1  // You can change the channel number as needed

#define SERIAL_PORT "dev/ttyS0" // change this port if needed

int main(void) {
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read;
    int serial_port;

    //Open the serial port
    serial.port = open(SERIAL_PORT, 0_RDWR | 0_NOCITY | 0_NDELAY);
    if (serial_port == -1){
        perror("Unable to open serial port");
	return 1;
    }

    // Allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set the local Bluetooth device address
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t)CHANNEL;

    // Bind the socket to the local address
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // Put the socket into listening mode
    listen(s, 1);

    // Accept one connection
    socklen_t opt = sizeof(rem_addr);
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str(&rem_addr.rc_bdaddr, buf);
    fprintf(stderr, "Accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

    // Read data from the client and send data over serial port
    while(1){
	bytes_read = read(client, buf, sizeof(buf));
        if (bytes_read > 0) {
        printf("Received: %s\n", buf);
        }
	const char *message = "Hello Serial World! \n";
	write(serial_port, message, strlen(message));
	wait(1000);
    }

    // Close the connection and the sockets
    close(client);
    close(s);

    return 0;
}
