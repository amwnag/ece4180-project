
#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include <stdio.h> 

uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
Serial  pi(USBTX, USBRX);
PwmOut speaker(p21);
AnalogIn pot(p20);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
PwmOut led4(LED4);

Mutex uLCD_mutex;

volatile int moist_data = 0;
volatile int i = 0;
volatile float volume;// = 1.0f;
volatile bool alarm = false;
int       len = 0;
 
void thread5(void const* args)
{
    while(true) {
        char temp = 0;
        char buffer[256];

        uLCD_mutex.lock();
        uLCD.locate(8, 1);
        uLCD.text_height(1);
        uLCD.text_width(1);
        if (pi.readable()) {
            pi.gets(buffer, sizeof(buffer));
            moist_data = atoi(buffer);
            led4 = 0;

        } else {
            led4 = 1;
        }

        uLCD_mutex.unlock();
        printf("Moisture data: %s\n", buffer);
        Thread::wait(100);
    }
    
}

void thread1(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

void thread2(void const *args) {
    while (true) {
        uLCD_mutex.lock();
        uLCD.locate(1, 1);
        uLCD.text_height(2);
        uLCD.text_width(2);
        uLCD.color(0x0390fc);
        uLCD.printf("%d", moist_data);
        if (!alarm) {

            uLCD.locate(1, 5);
            uLCD.text_height(1);
            uLCD.text_width(1);
            uLCD.printf(" :)");
        } else {
            uLCD.locate(1, 5);
            uLCD.text_height(1);
            uLCD.text_width(1);
            uLCD.printf(">:(");
        }
        uLCD_mutex.unlock();
        Thread::wait(100);
    }
}


void thread3(void const *args) {
    while (true) {
        if (alarm) {
            speaker = 0.5f * volume;
        } else {
            speaker = 0.0f;
        }
        Thread::wait(200);
    }
}


void thread4(void const *args) {
    while(true) {
        uLCD_mutex.lock();
        if (volume > 0.66) {
            uLCD.filled_rectangle(125, 5, 127, 11, WHITE);
        } else {
            uLCD.filled_rectangle(125, 5, 127, 11, BLACK);
        }
        if (volume > 0.33) {
            uLCD.filled_rectangle(120, 7, 122, 11, WHITE);
        } else {
            uLCD.filled_rectangle(120, 7, 122, 11, BLACK);
        }
        
        if (volume > 0.05) {
            uLCD.filled_rectangle(115, 9, 117, 11, WHITE);
        } else {
            uLCD.filled_rectangle(115, 9, 117, 11, BLACK);
        }

        uLCD_mutex.unlock();
            
        Thread::wait(100);
    }


}


int main()
{
    pi.baud(9600);

    speaker.period(1.0/150.0); // 500hz period
    volume = 0.0f;

    Thread t1(thread1);
    Thread t2(thread2);
    Thread t3(thread3);
    Thread t4(thread4);
    Thread t5(thread5);

    uLCD.cls();

    while (true) {
        volume = pot;
        if (moist_data > 50) {
            alarm = false;
        } else {
            alarm = true;
        }
        Thread::wait(500);
    }
}
