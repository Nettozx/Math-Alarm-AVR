/*
* lab5.c
*
* Created: 5/28/2014 4:30:04 PM
*  Author: Usman Majid
*  Partner: Thomas Le
*/

#include <avr/io.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#define keyport PORTA
#define keyportddr DDRA
#define keyportpin PINA

struct Time
{
	int sec;
	int min;
	int hour;
	int day;
	int month;
	int year;
}time;

struct Alarm
{
	int min;
	int hour;
	bool alarmOn;
	bool alarmRinging;
	int arithRand;
}alarm;

struct Math
{
	int difficulty;
	int var1;
	int var2;
	int ans;
	int guess;
}math;

void init(void)
{
	DDRA  = 0x0f;		//Key-pad port, most significant bits(msb) - input, least significant bits(lsb) - output
	PORTA = 0xff;		//pull-up enabled for msb
	ini_lcd();
	SET_BIT(DDRB, PINB3);
}
/* 
  Read random seed from eeprom and write a new random one. 
*/ 
void initrand() 
{ 
        uint32_t state; 
        static uint32_t EEMEM sstate; 

        state = eeprom_read_dword(&sstate); 

        // Check if it's unwritten EEPROM (first time). Use something funny 
        // in that case. 
        if (state == 0xffffffUL) 
                state = 0xDEADBEEFUL; 
        srandom(state); 
        eeprom_write_dword(&sstate, rand()); 
}
 
int days_of_month[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
int days_of_month_leap[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
bool editMode = false; //edit settings
bool resetFlag = false; //reset flag
bool alarmSetFlag = false; //alarm flag
bool alarmHasBeenSet = false;
void reset(struct Time*);
bool isLeapYear(int);
void increment_time(struct Time*);
unsigned char keypad();
int power(int base, int exp);
int random_in_range (unsigned int min, unsigned int max);
bool inputtingDone = false;
bool specialChar = false;

int main(void)
{
	init();
	initrand();
	SET_BIT(DDRD,PIND4); // set pinB0 as output
	time.sec = 0;
	time.min = 0;
	time.hour = 0;
	time.day = 1;
	time.month = 1;
	time.year = 1000;
	alarm.hour = 0;
	alarm.min = 0;
	alarm.alarmOn = false;
	alarm.alarmRinging = false;
	alarm.arithRand = 1;
	math.var1 = 1;
	math.var2 = 1;
	math.ans = 1;
	math.guess = 0;
	unsigned char keyPressed;
	char date[10]; //buffer for displaying date
	char buf[9]; //buffer for displaying time
	char mathbuf[16];//buffer for math
	char ansbuf[16]; //buffer for math answer
	char temp[16]; //buffer for editing
	int tempidx = 0; //index for editing buffer
	
	//flags for editing
	bool settingYear = false;
	bool settingMonth = false;
	bool settingDay = false;
	bool settingHour = false;
	bool settingMin = false;
	bool settingSec = false;
	bool settingDifficulty = false;
	
	while(1)
	{
		if(resetFlag)
		{reset(&time);
		resetFlag = false;}
		keyPressed = keypad();

		if(!editMode && !alarmSetFlag) {
			sprintf(date, "%02i/%2i/%4i", time.day, time.month, time.year);
			pos_lcd(0,0);
			put_str_lcd(date);
			sprintf(buf, "%02i:%02i:%02i", time.hour, time.min, time.sec);
			pos_lcd(1,0);
			put_str_lcd(buf);
			increment_time(&time);
			if(!alarm.alarmRinging && alarmHasBeenSet && time.hour == alarm.hour && time.min == alarm.min)
			{
				//set bit to second micro controller to start sounding alarm
				SET_BIT(PORTD, PIND4);
				pos_lcd(0,0);
				put_str_lcd("Alarm Armed!");
				increment_time(&time);
				alarm.alarmRinging = true;
			}
			if(alarm.alarmRinging)
			{
				bool q1 = true;
				//bool q2 = true;
				//bool q3 = true;
				math.var1 = random_in_range(1,power(10,math.difficulty));
				math.var2 = random_in_range(1,power(10,math.difficulty));
				if(alarm.arithRand == 0)
				{
					math.ans = math.var1 + math.var2;
					sprintf(mathbuf, "%i+%i=", math.var1, math.var2);	
				}
				else if(alarm.arithRand == 1)
				{
					if(math.var1 < math.var2)
					{
						math.ans = math.var2 - math.var1;
						sprintf(mathbuf, "%i-%i=", math.var2, math.var1);	
					}
					else
					{
						math.ans = math.var1 - math.var2;
						sprintf(mathbuf, "%i-%i=", math.var1, math.var2);
					}
				}
				else if(alarm.arithRand == 2)
				{
					math.ans = math.var1 * math.var2;
					sprintf(mathbuf, "%i*%i=", math.var1, math.var2);
				}
				clr_lcd();
				pos_lcd(0,0);
				put_str_lcd(mathbuf);
				pos_lcd(1,0);
				tempidx = 0;
				while(q1 && alarm.alarmRinging)
				{
					keyPressed = keypad();
					if(keyPressed != 0xff){
						int copyOftempIdx = tempidx-1;
						/////////////////////////////////////////
						if(inputtingDone)
						{
							//math.guess = (((temp[0]-48)*10) + (temp[1]-48));
							for(int i = 0; i < tempidx; i++)
							{
								math.guess += ((temp[i]-'0')*power(10,copyOftempIdx));
								copyOftempIdx--;
							}
							if(math.guess != math.ans){
								clr_lcd();
								put_str_lcd("Wrong Answer!");
								wait_avr(16000);
								clr_lcd();
								tempidx = 0;
								inputtingDone = false;
								pos_lcd(0,0);
								put_str_lcd(mathbuf);
								pos_lcd(1,0);
								//sprintf(ansbuf, "=%i", math.guess);
								//put_str_lcd(ansbuf);
								math.guess = 0;
							}
							else{
								clr_lcd();
								put_str_lcd("Correct Answer!");
								wait_avr(16000);
								clr_lcd();
								increment_time(&time);
								CLR_BIT(PORTD, PIND4);
								alarm.alarmRinging = false;
								alarmHasBeenSet = false;
								q1 = false;
								inputtingDone = false;
								math.guess = 0;
								clr_lcd();
							}
						}
						////////////////////////////////////////
						if(!specialChar)
						{
							put_lcd(keyPressed);
							temp[tempidx] = keyPressed;
							tempidx++;
						}
					}
				}
			}
		}
		else if(alarmSetFlag)
		{
			clr_lcd();
			settingMin = true; settingHour = true; settingDifficulty = true;
			alarm.arithRand = time.sec%3;
			pos_lcd(0,0);
			put_str_lcd("Set Alarm Hour: ");
			pos_lcd(1,0);
			tempidx = 0;
			while(settingHour && alarmSetFlag) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						alarm.hour = (((temp[0]-48)*10) + (temp[1]-48));
						if(alarm.hour > 23){
							clr_lcd();
							put_str_lcd("Invalid Hour");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Alarm Hour:   ");
							pos_lcd(1,0);
						}
						else{
							settingHour = false;
							clr_lcd();
						}
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Alarm Min: ");
			pos_lcd(1,0);
			while(settingMin && alarmSetFlag) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						alarm.min = (((temp[0]-48)*10) + (temp[1]-48));
						if(alarm.min > 59){
							clr_lcd();
							put_str_lcd("Invalid Min");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Alarm Min:   ");
							pos_lcd(1,0);
						}
						else{
							settingMin = false;
							clr_lcd();
						}
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			put_str_lcd("Set Diff(1-5): ");
			pos_lcd(1,0);
			tempidx = 0;
			while(settingDifficulty && alarmSetFlag) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx >= 1)
					{
						math.difficulty = (temp[0]-48);
						if(math.difficulty > 5){
							clr_lcd();
							put_str_lcd("Invalid Difficulty");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Difficulty:   ");
							pos_lcd(1,0);
						}
						else{
							settingDifficulty = false;
							clr_lcd();
						}
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			alarmHasBeenSet = true;
			alarm.alarmRinging = false;
			alarmSetFlag = false;
		}
		///////////////////////////////////////////////////////////////////
		else {
			clr_lcd();
			settingYear = true; settingMonth = true; settingDay = true;
			settingHour = true; settingMin = true; settingSec = true;
			//////////////////////////////////////////////////////////////////////////
			
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			put_str_lcd("Set Year: ");
			pos_lcd(1,0);
			tempidx = 0;
			while(settingYear && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 3)
					{
						time.year = (((temp[0]-48)*1000) + ((temp[1]-48)*100) + ((temp[2]-48)*10) + (temp[3]-48));
						if(time.year > 9999){
							clr_lcd();
							put_str_lcd("Invalid Year");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Year:    ");
							pos_lcd(1,0);
						}
						else{
							settingYear = false;
							clr_lcd();
						}
						//editMode = false; //remove later
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Month: ");
			pos_lcd(1,0);
			while(settingMonth && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						time.month = (((temp[0]-48)*10) + (temp[1]-48));
						if(time.month > 12){
							clr_lcd();
							put_str_lcd("Invalid Month");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Month:   ");
							pos_lcd(1,0);
						}
						else{
							settingMonth = false;
							clr_lcd();
						}
						//editMode = false; //remove later
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Day: ");
			pos_lcd(1,0);
			while(settingDay && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						time.day = (((temp[0]-48)*10) + (temp[1]-48));
						if(isLeapYear(time.year)){
							if(time.day > days_of_month_leap[time.month]){
								clr_lcd();
								put_str_lcd("Invalid Day");
								wait_avr(1600);
								tempidx = 0;
								pos_lcd(0,0);
								put_str_lcd("Set Day:   ");
								pos_lcd(1,0);
							}
							else{
								settingDay = false;
								clr_lcd();
							}
						}
						else{
							if(time.day > days_of_month[time.month]){
								clr_lcd();
								put_str_lcd("Invalid Day");
								wait_avr(1600);
								tempidx = 0;
								pos_lcd(0,0);
								put_str_lcd("Set Day:   ");
								pos_lcd(1,0);
							}
							else{
								settingDay = false;
								clr_lcd();
							}
						}
						
						//editMode = false; //remove later
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Hour: ");
			pos_lcd(1,0);
			while(settingHour && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						time.hour = (((temp[0]-48)*10) + (temp[1]-48));
						if(time.hour > 23){
							clr_lcd();
							put_str_lcd("Invalid Hour");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Hour:   ");
							pos_lcd(1,0);
						}
						else{
							settingHour = false;
							clr_lcd();
						}
						//editMode = false; //remove later
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Min: ");
			pos_lcd(1,0);
			while(settingMin && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						time.min = (((temp[0]-48)*10) + (temp[1]-48));
						if(time.min > 59){
							clr_lcd();
							put_str_lcd("Invalid Min");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Min:   ");
							pos_lcd(1,0);
						}
						else{
							settingMin = false;
							clr_lcd();
						}
						//editMode = false; //remove later
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pos_lcd(0,0);
			tempidx = 0;
			put_str_lcd("Set Sec: ");
			pos_lcd(1,0);
			while(settingSec && editMode) {
				keyPressed = keypad();
				if(keyPressed != 0xff){
					put_lcd(keyPressed);
					temp[tempidx] = keyPressed;
					tempidx++;
					if(tempidx > 1)
					{
						time.sec = (((temp[0]-48)*10) + (temp[1]-48));
						if(time.sec > 59){
							clr_lcd();
							put_str_lcd("Invalid Sec");
							wait_avr(1600);
							tempidx = 0;
							pos_lcd(0,0);
							put_str_lcd("Set Sec:   ");
							pos_lcd(1,0);
						}
						else{
							settingSec = false;
							clr_lcd();
						}
						editMode = false; //end editing
					}
				}
			}
		}
	}//end of while(1)
	//LAB 2 END
}
unsigned char keypad()
{
	unsigned char MSB, keyCode, keyPressed, i;
	MSB = 0xff;
	
	for(i=0; i<4; i++)
	{
		wait_avr(1);					  //delay for port
		keyport = ~(0x01 << i);
		wait_avr(1);  		  	 		  //delay for port
		MSB = keyportpin | 0x0f;		  //detect key press
		
		if (MSB != 0xff)				  //if a key is pressed
		{
			wait_avr(20); 		  		  //key debouncing delay
			MSB = keyportpin | 0x0f;	  //set to row
			if(MSB == 0xff) goto OUT;	  //if no key pressed go to OUT label
			
			keyCode = (MSB & 0xf0) | (0x0f & ~(0x01 << i)); //find out which key was pressed in hex
			
			while (MSB != 0xff)			  //while key is pressed
			MSB = keyportpin | 0x0f;	  //set to key
			
			wait_avr(20);   			  //key debouncing delay
			
			switch (keyCode)			  //convert hex key location to character (commented out unused keys)
			{
				case (0xee): keyPressed = '1'; specialChar = false;
				break;
				case (0xde): keyPressed = '2'; specialChar = false;
				break;
				case (0xbe): keyPressed = '3'; specialChar = false;
				break;
				case (0x7e): /*keyPressed = 'A';*/ editMode = true; specialChar = true;
				break;
				case (0xed): keyPressed = '4'; specialChar = false;
				break;
				case (0xdd): keyPressed = '5'; specialChar = false;
				break;
				case (0xbd): keyPressed = '6'; specialChar = false;
				break;
				case (0x7d): /*keyPressed = 'B';*/ clr_lcd(); editMode = false; specialChar = true;
				break;
				case (0xeb): keyPressed = '7'; specialChar = false;
				break;
				case (0xdb): keyPressed = '8'; specialChar = false;
				break;
				case (0xbb): keyPressed = '9'; specialChar = false;
				break;
				case (0x7b): /*keyPressed = 'C';*/ clr_lcd(); resetFlag = true; editMode = false; inputtingDone = false; specialChar = true;
				break;
				case (0xe7): /*keyPressed = '*';*/ specialChar = true;
				//pos_lcd(0,0);
				//if(!alarm.alarmOn)
				//{
					//put_str_lcd("Alarm On");
					//alarm.alarmOn = true;
				//}
				//else
				//{
					//put_str_lcd("Alarm Off");
					//alarm.alarmOn = false;
				//}
				break;
				case (0xd7): keyPressed = '0'; specialChar = false;
				break;
				case (0xb7): /*keyPressed = '#';*/ specialChar = true;
					inputtingDone = true;
				break;
				case (0x77): /*keyPressed = 'D';*/ alarmSetFlag = true; inputtingDone = false; clr_lcd(); specialChar = true;
				break;
				default	   : /*keyPressed = 'X';*/
				break;
			}//end of switch
			
			//put_lcd(keyPressed); //output key to LCD
			
			OUT:;
			return keyPressed;
		}//end of if
	}//end of for
}

void reset(struct Time* time)
{
	time->year = 1000;
	time->month = 1;
	time->day = 1;
	time->hour = 0;
	time->min = 0;
	time->sec = 0;
}

bool isLeapYear(int y)
{
	if(y % 4 == 0) {
		if(y % 100 == 0) {
			if(y % 400 == 0) {
				return true;
			}
			else
			return false;
		}
		else
		return true;
	}
	else
	return false;
}

void increment_time(struct Time* time)
{
	time->sec++;
	wait_avr(8000);
	if(time->sec > 59) {
		time->min++;
		time->sec = 0;
		if(time->min > 59) {
			time->hour++;
			time->min = 0;
			if(time->hour > 23) {
				time->day++;
				time->hour = 0;
				if(isLeapYear(time->year)) {
					if(time->day > days_of_month_leap[time->month]) {
						time->month++;
						time->day = 1;
						if(time->month > 12) {
							time->year++;
							time->month = 1;
						}
					}
					} else {
					if(time->day > days_of_month[time->month]) {
						time->month++;
						time->day = 1;
						if(time->month > 12) {
							time->year++;
							time->month = 1;
						}
					}
				}
			}
		}
	}
}

int power(int base, int exp) {
	if (exp == 0)
	return 1;
	else if (exp % 2)
	return base * power(base, exp - 1);
	else {
		int temp = power(base, exp / 2);
		return temp * temp;
	}
}

int random_in_range (unsigned int min, unsigned int max)
{
	return(rand()%max+min);
}