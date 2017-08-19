#Fingerprint Scanner Lock

This is a Fingerprint scanner lock written in the Arduino IDE for the Arduino Uno.

I first attempted to write this using pure C with the avr-libc but interfacing the Arduino with the fingerprint scanner in pure C was too difficult. The C code is included in this repository.

In order to reproduce this project you will need the following Items:
* An Arduino Uno 
* A 3 * 4 Matrix Keypad 
* An Adafruit Fingerprint Scanner

Dependencies:
* Adafruit-Fingerprint-Sensor-Library https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
* Matrix Keypad Library http://playground.arduino.cc/Code/Keypad

AVR-C Dependencies:
* AVR UART https://github.com/andygock/avr-uart
* AVR Software-UART https://github.com/blalor/avr-softuart
