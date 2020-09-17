// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: March 30, 2018
// Last Modified:  change this or look silly
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R |= 0x20;
	volatile int nop;
	nop++;
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0xFF;
	GPIO_PORTF_DIR_R &= 0xEF;
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTF_DEN_R |= 0x14;
	
	GPIO_PORTF_PUR_R |= 0x10;
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- PF2 is heartbeat
	GPIO_PORTF_DATA_R ^= 0x04; //make heartbeat
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press

//	while((GPIO_PORTF_DATA_R & 0x10) == 0x10){} //constantly loop while waiting for release of switch
	int wait = 1;
	int waitO = 1;
	while(wait){
	if((GPIO_PORTF_DATA_R & 0x10) == 0x10){
		int n = 20; 																//delay of 20 ms
		wait--;
		while(n){
			int time = 72724*2/91;  // 1msec, tuned at 80 MHz
			while(time){
				time--;
			}
			n--;
		}
	}
	}
	//while((GPIO_PORTF_DATA_R & 0x10) != 0x10){}	//constantly loop while waiting for touch of switch
	while(waitO){
		if((GPIO_PORTF_DATA_R & 0x10) != 0x10){
			waitO--;
			
		}
	}
}  

