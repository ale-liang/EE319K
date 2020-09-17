// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"
#include "music.h"
const unsigned short wave[16] = {8, 13, 15, 14, 13, 12, 13, 10, 5, 1, 0, 2, 3, 2, 3, 7};
uint8_t idx = 0;
float multiplierVolume=1;
float dec = 0;

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();								//Initialize digital outputs (Port B)
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0xFFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x0005;	
}


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	if(period > 0){
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = period;
		NVIC_ST_CURRENT_R = 0;
		NVIC_ST_CTRL_R = 0x0007;
	}
	else{
		NVIC_ST_CTRL_R = 0;
		//NVIC_ST_RELOAD_R = 0;
		//NVIC_ST_CURRENT_R = 0;
		//NVIC_ST_CTRL_R = 0x05;
	}
	idx = 0;
		
}

void Envelope(void){
	dec += 0.01; 
}

void SysTick_Handler(void){
	//DAC_Out((wave[idx] * (multiplierVolume - dec)));
	DAC_Out(wave[idx]);
	idx = ((idx + 1) % 16);
}

