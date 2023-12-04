#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define SERVER_ADDRESS "B8:27:EB:12:D7:91"  // Replace with the Bluetooth address of your target device
#define LED 17 //LED pin is GPIO_17
/*
Plant module behavior
1. Connect to desk via bluetooth on boot
    - debug external LED blinking
2. Verify connection
    - debug external LED solid
3. Read data from ADC (which receives input from moisture sensor)
4. Transmit data via bluetooth every second
Repeat 3-4 until power off.
*/

// Called when CTL C or STOP button hit
static void err_handler (int sig){
	gpioTerminate(); //release GPIO locks & resources
	signal(SIGINT, SIG_DFL); //exit program
	kill(getppid(), SIGINT); //kill it off
	kill(getpid(), SIGINT);
	exit(0);
}
static void exit_handler(void) {
	gpioTerminate(); //release GPIO locks & resources on exit
}

int main() {
    // gpio library init
    if (gpioInitialise()<0) return 1; // init I/O library
    signal (SIGQUIT, err_handler);// CTL C and STOP button
    signal (SIGINT, err_handler); // GPIO exit & cleanup
    signal (SIGTERM, err_handler);
    signal (SIGABRT, err_handler);
    atexit(exit_handler);  // exit handler cleanup 
    
    // gpio code, can opt to make LED blink
    gpioSetMode(LED, PI_OUTPUT); // set LED pin to output
    
    // bluetooth init
    struct sockaddr_rc addr = {0};
    int s, status;
    // char message[] = "Hello, Bluetooth!";

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
    
    
    // Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
    
    // Get I2C device, ADS1115 I2C address is 0x48(72)
	ioctl(file, I2C_SLAVE, 0x48);

	// Select configuration register(0x01)
	// AINP = AIN0 and AINN = AIN1, +/- 2.048V
	// Continuous conversion mode, 128 SPS(0x84, 0x83)
	char config[3] = {0};
	config[0] = 0x01;
	config[1] = 0x84;
	config[2] = 0x83;
	write(file, config, 3);
	sleep(1);
    
    // Connect to the bluetooth server
    status = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if (status == -1) {
        perror("Connection failed");
        close(s);
        exit(EXIT_FAILURE);
    }
    
    
    // WE ARE NOW CONNECTED!
    gpioWrite  (LED, PI_ON);  // LED on
    
    // READ AND SEND UNTIL POWER OFF (or interrupt if desired)
    
    // WIP BELOW
    
 	// Read 2 bytes of data from register(0x00)
	// raw_adc msb, raw_adc lsb
	char reg[1] = {0x00};
	write(file, reg, 1);
	char data[2]={0};
	char buffer[10]={0}; // "0" to "100", uhhh check if this is a good number
	// alternatively can send the smaller char array over bt then do the operations on desk pi
	// probably preferable, as we are transmitting smaller sizes of data
	// or send int raw_data encoded as char, convert back on desk pi
	
	if(read(file, data, 2) != 2)
	{
		printf("Error : Input/Output Error \n");
	}
	else 
	{
		// Convert the data
		int raw_adc = (data[0] * 256 + data[1]);
		if (raw_adc > 32767)
		{
			raw_adc -= 65535; // value range from 5000 to 15000
		}

		
		// Output data to screen
		// printf("Digital Value of Analog Input: %d \n", raw_adc);
		
        // message as char array
        
        sprintf(buffer, "%d\n", (raw_adc-5000)/100);
        // Send data
        status = write(s, buffer, sizeof(buffer));
        if (status == -1) {
            perror("Data transmission failed");
        } else {
            printf("Data sent successfully: %s\n", buffer);
            
        }   
	}   

 
    
    close(s); // Close the socket
    gpioTerminate();
    
    return 0;
}
