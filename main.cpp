#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include <stdio.h> 
#include "icons.h"
#include "Speaker.h"
#include "pwm_tone.h"

uLCD_4DGL uLCD(p9,p10,p11); // uLCD Screen
Serial  pi(USBTX, USBRX);   // Serial device for desk pi
AnalogIn pot(p20);          // Volume potentiometer
PwmOut Buzzer(p26);         // Alarm Buzzer

DigitalOut led1(LED1);      // LED to verify that threads are running
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);      // LED to verify pi communication - 1 if receiving

Mutex uLCD_mutex;

volatile int moist_data;    // Global var for moisture data
volatile bool screen_mode;

volatile float volume;      // Global var for volume
volatile bool alarm_mode;    // Global var for alarm mode
volatile bool received;

// Draws the sad face
int face_rad = 40;
int face_x = 64;
int face_y = 64;
float G_5 = 1000000.0/So5,Gs_5 = 1000000.0/So5s,A_5 = 1000000.0/La5;

//draws frown line
void generateFrown(int face_x, int face_y, int color) {
    int frown_diagonal = 5;
    int frown_offset_x = 16;
    int frown_offset_y = 15;
    int frown_line = 24;

    uLCD.line((face_x - 30), 50, (face_x - 20), 40, color);
    uLCD.line((face_x + 30), 50, (face_x + 20), 40, color);

    for (int i = 0; i < frown_diagonal; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), color);
        frown_offset_x--;
        frown_offset_y--;
    }   
    for (int i = 0; i < frown_line; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), color);
        frown_offset_x--;
    }

    for (int i = 0; i < frown_diagonal; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), color);
        frown_offset_x--;
        frown_offset_y++;
    }
   
}

// Draws smile line
void generateSmile(int face_x, int face_y, int color) {
    int smile_diagonal = 5;
    int smile_offset_x = 16;
    int smile_offset_y = -15;
    int smile_line = 24;

    for (int i = 0; i < smile_diagonal; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), color);
        smile_offset_x--;
        smile_offset_y--;
    }   
    for (int i = 0; i < smile_line; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), color);
        smile_offset_x--;
    }

    for (int i = 0; i < smile_diagonal; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), color);
        smile_offset_x--;
        smile_offset_y++;
    }
}

// Debug thread - if blinking, threads are running
void thread1(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

// Updates smiley based on moisture readings
void thread2(void const *args) {
    while (true) {
        if (screen_mode != alarm_mode) {
            uLCD_mutex.lock();

            if (!alarm_mode) {
                //uLCD.color(0x0390fc);
                //uLCD.printf(" %d  ", moist_data);
                uLCD.filled_rectangle(5, 105, 21, 122, BLACK);
                generateFrown(face_x, face_y, BLACK);
                generateSmile(face_x, face_y, WHITE);
            } else {
                //uLCD.color(RED);
                //uLCD.printf(" %d  ", moist_data);
                uLCD.BLIT(5,105,16,16, water_drop);
                generateSmile(face_x, face_y, BLACK);
                generateFrown(face_x, face_y, WHITE);
            }
            screen_mode = alarm_mode;
            uLCD.color(0x0390fc);
            uLCD_mutex.unlock();
        }
        Thread::wait(1000);
    }
}

// Sounds buzzer based on alarm mode
void thread3(void const *args) {
    while (true) {
        if (alarm_mode & received) {
            Tune(Buzzer,G_5, 4, volume * 0.5);
        } else if (received) {
            Tune(Buzzer, 0.0, 1/16, volume * 0.5);
        } 
        Thread::wait(2000);
    }
}


// Reads volume level from pot and updates volume icons
void thread4(void const *args) {
    while(true) {
        uLCD_mutex.lock();
        if (volume > 0.66) {
            uLCD.filled_rectangle(120, 14, 122, 5, WHITE);
        } else {
            uLCD.filled_rectangle(120, 14, 122, 5, BLACK);
        }
        if (volume > 0.33) {
            uLCD.filled_rectangle(115, 14, 117, 8, WHITE);
        } else {
            uLCD.filled_rectangle(115, 14, 117, 8, BLACK);
        }
        
        if (volume > 0.03) {
            uLCD.filled_rectangle(110, 14, 112, 11, WHITE);
            uLCD.BLIT(85,0,18,18, volume_on);

        } else {
            uLCD.filled_rectangle(110, 14, 112, 11, BLACK);
            uLCD.BLIT(85,0,18,18, volume_off);
        }
        uLCD_mutex.unlock();
            
        Thread::wait(100);
    }
}

// Read serial data from desk pi and decode moisture levels
void thread5(void const* args) {
    while(true) {
        char buffer[2] = {0};
        int raw_adc;

        if (pi.readable()) {
            received = true;
            // Get two bytes of data over serial
            buffer[0] = pi.getc();
            buffer[1] = pi.getc();

            // Convert data to a range 5000-15000
            raw_adc = (buffer[0] * 256 + buffer[1]);
            if (raw_adc > 32767) {
                raw_adc -= 65535; // value range from 5000 to 15000
            }
            // Scale to 0-100 to display on the pi
            
            if ((raw_adc < 3000)) {
                moist_data = 0;
            } else if (raw_adc > 13000) {
                moist_data = 100;
            } else if (raw_adc >= 3000) {
                moist_data =(int)(double(raw_adc - 3000) / 100.0 + 0.5);
            }
            
            led4 = 1;
        } else {
            led4 = 0;
        }

        uLCD_mutex.lock();
        uLCD.locate(5, 15);
        uLCD.text_height(1);
        uLCD.text_width(1);
        //uLCD.printf("   %d   ", raw_adc); // DEBUG - can delete

        uLCD.locate(1, 1);
        if (!alarm_mode & received) {
            uLCD.color(0x0390fc);
            uLCD.printf(" %d  ", moist_data);
        } else if (received) {
            uLCD.color(RED);
            uLCD.printf(" %d  ", moist_data);
        }
        uLCD_mutex.unlock();
        //printf("Moisture data: %s\n", buffer);
        Thread::wait(500);
    }
}


int main() {
    pi.baud(9600);              // Set pi serial baud rate
    uLCD.cls();

    Buzzer.period(1.0/150.0);   // 500hz period

    // Initialize global vars
    volume = 0.0f;
    //moist_data = 0;
    screen_mode = true;
    alarm_mode = true;
    received = false;

    //uLCD.BLIT(40,30,16,16, water_drop);
    //uLCD.BLIT(80,28,18,18, light_bulb);

    // Draw smiley circle + eyes
    uLCD_mutex.lock();
    uLCD.circle(face_x, face_y, face_rad, WHITE);
    uLCD.filled_circle((face_x - 19), 60, 8, WHITE);
    uLCD.filled_circle((face_x + 19), 60, 8, WHITE);
    uLCD.line((face_x - 30), 50, (face_x - 20), 40, WHITE);
    uLCD.line((face_x + 30), 50, (face_x + 20), 40, WHITE);
    uLCD_mutex.unlock();

    // Initialize threads
    Thread t1(thread1);
    Thread t2(thread2);
    Thread t3(thread3);
    Thread t4(thread4);
    Thread t5(thread5);

    while (true) {
        // Set alarm mode based on moisture readings
        volume = pot;
        if (received) {
            if (moist_data >= 50) {
                alarm_mode = false;
            } else {
                alarm_mode = true;
            }
        }
        
        
        Thread::wait(500);
    }
}
