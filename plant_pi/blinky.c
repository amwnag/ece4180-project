#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>

#define LED 17 //LED pin is GPIO_17

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
int main(int argc, char *argv[])
{
   if (gpioInitialise()<0) return 1; // init I/O library
   signal (SIGQUIT, err_handler);// CTL C and STOP button
   signal (SIGINT, err_handler); // GPIO exit & cleanup
   signal (SIGTERM, err_handler);
   signal (SIGABRT, err_handler);
   atexit(exit_handler);  // exit handler cleanup 
   //IO code starts here
   gpioSetMode(LED, PI_OUTPUT); // set LED pin to output
    while(1){
	gpioWrite  (LED, PI_ON);  // LED on
	time_sleep(0.25);         // Delay .25 seconds
	gpioWrite  (LED, PI_OFF); // LED off
	time_sleep(0.25);         // Delay .25 seconds
   }
   gpioTerminate();
   return 0;
}
