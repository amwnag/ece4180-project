#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include <stdio.h> 
#include "icons.h"
#include "Speaker.h"

#include "pwm_tone.h"

uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
Serial  pi(USBTX, USBRX);
//Speaker mySpeaker(p26);
PwmOut Buzzer(p26);
AnalogIn pot(p20);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

Mutex uLCD_mutex;

volatile int moist_data = 0;
volatile int i = 0;
volatile float volume = 0.8;// = 1.0f;
volatile float potValue;

volatile bool alarm = false;
int       len = 0;

//draws sad face
    
int face_rad = 40;
    int face_x = 64;
    int face_y = 64;
 float G_5 = 1000000/So5,
       Gs_5 = 1000000/So5s,
       A_5 = 1000000/La5;



void setVolume(float newVolume) {
 volume = newVolume;
}

//draws frown line
void generateFrown(int face_x, int face_y) {
    int frown_diagonal = 5;
    int frown_offset_x = 16;
    int frown_offset_y = 15;
    int frown_line = 24;
    

    for (int i = 0; i < frown_diagonal; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), WHITE);
        frown_offset_x--;
        frown_offset_y--;
    }   
    for (int i = 0; i < frown_line; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), WHITE);
        frown_offset_x--;
    }

    for (int i = 0; i < frown_diagonal; ++i){
        uLCD.pixel((face_x - frown_offset_x), (face_y + frown_offset_y), WHITE);
        frown_offset_x--;
        frown_offset_y++;
    }
   
}

void generateSmile(int face_x, int face_y) {
    int smile_diagonal = 5;
    int smile_offset_x = 16;
    int smile_offset_y = 5;
    int smile_line = 24;
     for (int i = 0; i < smile_diagonal; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), WHITE);
        smile_offset_x--;
        smile_offset_y++;
    }   
    for (int i = 0; i < smile_line; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), WHITE);
        smile_offset_x--;
    }

    for (int i = 0; i < smile_diagonal; ++i){
        uLCD.pixel((face_x - smile_offset_x), (face_y - smile_offset_y), WHITE);
        smile_offset_x--;
        smile_offset_y--;
    }
}

//Mbed LED control
void thread1(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}
//display moisture status 
void thread2(void const *args) {
    while (true) {
        uLCD_mutex.lock();
        uLCD.locate(1, 1);
        uLCD.text_height(2);
        uLCD.text_width(2);
        uLCD.color(0x0390fc);
        uLCD.printf("%d", moist_data);
        if (!alarm) {

            // uLCD.locate(1, 5);
            // uLCD.text_height(1);
            // uLCD.text_width(1);
            // uLCD.printf(" :)");
            generateSmile(face_x, face_y);
        } else {
            // uLCD.locate(1, 5);
            // uLCD.text_height(1);
            // uLCD.text_width(1);
            // uLCD.printf(">:(");
            generateFrown(face_x, face_y);
        }
        uLCD_mutex.unlock();
        Thread::wait(100);
    }
}

//speaker control
void thread3(void const *args) {
    while (true) {
        if (alarm) {
            Tune(Buzzer,G_5, 4);
        } else {
            Tune(Buzzer, 0.0, 1/16);
        }
        
        Thread::wait(200);
    }
}

//uLCD volume bar
void thread4(void const *args) {
    while(true) {
        
        uLCD_mutex.lock();
        uLCD.BLIT(85,0,18,18, volume_on);
        
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
        
        if (volume > 0.05) {
            uLCD.filled_rectangle(110, 14, 112, 11, WHITE);
            uLCD.BLIT(85,0,18,18, volume_on);

        } else {
            uLCD.filled_rectangle(110, 14, 112, 11, BLACK);
            uLCD.BLIT(85,0,18,18, volume_off);
        }
        
        uLCD_mutex.unlock();
        //setVolume(pot.read());
        Thread::wait(1000);
    }
}

//receive and read moisture data
void thread5(void const* args) {

    while(true) {
        char buffer[2] = {0};
        int raw_adc;


        uLCD_mutex.lock();
        uLCD.locate(5, 14);
        uLCD.text_height(1);
        uLCD.text_width(1);
        if (pi.readable()) {
            buffer[0] = pi.getc();
            buffer[1] = pi.getc();

            raw_adc = (buffer[0] * 256 + buffer[1]);
            if (raw_adc > 32767) {
                raw_adc -= 65535; // value range from 5000 to 15000
            }

            if (raw_adc < 5000) {
                moist_data = 0;
            } else if (raw_adc > 15000) {
                moist_data = 100;
            } else {
                moist_data =(int)(double(raw_adc - 5000) / 100.0 + 0.5);
            }

            uLCD.printf("   %d   ", raw_adc);
            led4 = 0;
        } else {
            led4 = 1;
        }

        uLCD_mutex.unlock();
        

        //printf("Moisture data: %s\n", buffer);
        Thread::wait(100);
    }
    
}

int main()
{
    pi.baud(9600);
    uLCD.cls();

    Buzzer.period(1.0/150.0); // 500hz period
    volume = 0.5f;
    //uLCD.BLIT(40,30,16,16, water_drop);
    //uLCD.BLIT(80,28,18,18, light_bulb);

    //draws head of emoticon
    uLCD_mutex.lock();
    uLCD.circle(face_x, face_y, face_rad, WHITE);
    uLCD.filled_circle((face_x - 19), 60, 8, WHITE);
    uLCD.filled_circle((face_x + 19), 60, 8, WHITE);
    uLCD.line((face_x - 30), 50, (face_x - 20), 40, WHITE);
    uLCD.line((face_x + 30), 50, (face_x + 20), 40, WHITE);
    uLCD_mutex.unlock();
    
    Thread t1(thread1);
    Thread t2(thread2);
    Thread t3(thread3);
    Thread t4(thread4);
    Thread t5(thread5);


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
