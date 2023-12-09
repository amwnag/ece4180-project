# ece4180-project
ECE4180 project Fall2023

Wiki: https://github.com/amwnag/ece4180-project/wiki

## Overview
**Team Members**: Amanda Wang, Devaughn Menezes, Eileen Liu, Sophia Griffith

![Picture](https://github.com/amwnag/ece4180-project/assets/86573349/1ef2064d-2348-4ed4-bd84-321e0e9dcc0f)


This project aims to create an interactive desk toy that remotely monitors soil conditions of a plant nearby. The LCD screen displays information about soil moisture, connectivity, and audio levels. If the moisture levels are below a specific threshold, a frown is shown and an alarm sounds every few seconds. The volume of this alarm is controlled through an input dial on the desk buddy.
<br/><br/> 
![Project Control Flow](https://github.com/amwnag/ece4180-project/blob/desk/images/PlantBuddyOverview.drawio.png)
<br/><br/> 
Three microcontrollers were used along with peripheral hardware elements to implement the desk buddy. One Raspberry Pi Zero W (Plant Pi) reads in moisture data from the sensor and a connected ADC. The Plant Pi communicates this data to another Raspberry Pi Zero W (Desk Pi) over Bluetooth. An mbed microcontroller receives the same data from the Desk Pi over a serial connection, and then updates the user interface display.

## Demo Video

[![Watch the video](http://i3.ytimg.com/vi/UxZS1Tiq9u8/hqdefault.jpg)](https://www.youtube.com/watch?v=UxZS1Tiq9u8)


## Parts List

**Hardware Components**
* 2x [Raspberry Pi Zero W](https://www.raspberrypi.com/products/raspberry-pi-zero-w/)
* 1x [mbed LPC 1768](https://os.mbed.com/platforms/mbed-LPC1768/)
* 1x [Potentiometer](https://www.sparkfun.com/products/9806)
* 1x [ADS1115](https://github.com/ControlEverythingCommunity/ADS1115/tree/master)
* 1x [Soil moisture sensor](https://github.com/sparkfun/Soil_Moisture_Sensor)
* 1x [uLCD display](https://os.mbed.com/users/4180_1/notebook/ulcd-144-g2-128-by-128-color-lcd/)
* 1x [Buzzer]
* 2x Power Supply
* 1x mini USB to micro USB cable

**Software**
* C/C++

## Setup Instructions

### Hardware Setup
**Mbed**:
1. Use the pinout diagrams below to connect the potentiometer, uLCD display, and buzzer to the mbed.
<br/><br/> 
![mbed Plant Desk Buddy pinout](https://github.com/amwnag/ece4180-project/blob/desk/images/mbed_pinout.drawio.png)
<br/><br/> 
2. When the Desk Pi setup is complete, connect the mbed to a USB port on the pi via the mini USB to micro USB cable. This will enable pi to mbed serial communication.

**Desk Pi**:
1. Power on the Desk Pi through a 5V power source.
2. Connect the Pi to the mbed via USB serial.

**Plant Pi**:
1. Connect the analog soil moisture sensor's output to the input A0 of the ADC (ADS1115)
2. 

### Software Setup
**Mbed**:
1. Download all the files in the mbed folder.
2. Create a new project in Keil Studio Cloud Compiler. Upload the files to the project.
3. With the mbed connected to the computer with Keil Studio opened via USB, build the project and download the build onto the mbed. 

**Desk Pi**:
1. Download `bluetooth_receive.c` and `startup.sh` from the desk_pi folder in the github repo.
2. Compile the C file with the line `gcc bluetooth_receive.c -o bluetooth_receive.o -Wall -lbluetooth`
3. If you would like the program to start at boot, do the following:
    1. In the terminal of the Desk Pi, enter the command `sudo nano /etc/rc.local`.
    2. Add the line `sudo sh /home/[user]/[location of files]/startup.h &` before `exit 0`.

**Plant Pi**:
1. Install libraries for bluetooth and Pi GPIO, if not already installed.
2. Compile the C code using the command `gcc plant_module.c -o plant_module.o -Wall -lbluetooth -lpigpio`
3. Once compiled to an executable, edit `/etc/rc.local` to include a command to run plant_module.o, making sure to use an absolute path (such as `/home/pichu/ece4180-project/plant_pi/plant_module.o`).



