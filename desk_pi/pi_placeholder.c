#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define SERIAL_PORT "/COM3"

int main(int argc, char ** argv) {
  int fd;
  char buf[8];
  int n;
  // Open the Port. We want read/write, no "controlling tty" status, 
  // and open it no matter what state DCD is in
  fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);  
  //open mbed's USB virtual com port
  if (fd == -1) {  
    perror("open_port: Unable to open /dev/ttyACM0 - ");
    return(-1);
  }

  //Send command to blink mbed led 10 times at one second rate
  //mbed code turns on led2 with a '1' and off with a '0'
  //mbed echoes back each character
  while (1) {
    char byte[8] = {0};
    int num = 20;
    sprintf(byte, "%d", num);
    write(fd, byte, strlen(byte));
    printf("byte length: %d", strlen(byte));
  }
  // Don't forget to clean up and close the port
  tcdrain(fd);
  close(fd);
  return 0;
}