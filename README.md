# Math-Alarm-AVR
CS145B Project 5
Usman Majid
Thomas Le

Arithmetic Alarm Clock
Description: This is an alarm clock that has a special way to turn it off,
 making sure that you will be awake after it goes off.
 The clock can be set by holding the A button down, it will ask for parameters
 (Year, Month, Day, Hour, Minute, Second) in that order. After that the alarm
 can be set by holding the D button, which will ask you for the hour and minute
 that the alarm should go off, followed by a difficulty parameter. The difficulty 
 parameter will dictate how challenging the arithmetic problems will be. The 
 difficulty can be set at 5 levels, level 1 being basic 1 to 2 digit arithmetic
 to level 5 being 5 digit arithmetic. The numbers for the math problem are randomized
 using the EEPROM state to load the seed for random(), then loading another random() 
 into EEPROM. The arithmetic is randomized by modulus 3 of the time.seconds when the
 alarm was set. This gives a fully randomized arithmetic alarm clock. 
 The second atmega32 used in this project is activated by a bit going live from the first
 atmega32 and the second microcontroller checks constantly if the bit is active. If it is
 it starts to play an annoying and loud run of all the octaves at a fast tempo. There is 
 a potentiometer next to the speaker where the volume can be set.

There are 2 project files included:
Lab5 - is the first atmega32 with the time and math logic
Lab5b - is the second atmega32 with the sound logic
 
 Demo URL: https://www.youtube.com/watch?v=GysNLDzMELc
