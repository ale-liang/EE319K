// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/17/2020 

// Specifications:
// Measure distance using slide pot, sample at 10 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.01cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// 

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on ST7735
// main3 adds your convert function, position data is no ST7735

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats

uint32_t ADCMail;
uint32_t ADCStatus;

void PortF_Init(void){ volatile int nop;
	SYSCTL_RCGCGPIO_R |= 0x20; //turn on clock for port F
	nop++;
	nop++;
	GPIO_PORTF_DIR_R |= 0x0E; //PF1-3 are outputs
	GPIO_PORTF_DEN_R |= 0x0E; //enable digital PF1-3
	GPIO_PORTF_PDR_R |= 0x0E; //enable pulldown resistors for PF1-3
}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm


uint32_t time0,time1,time2,time3;
uint32_t ADCtime,OutDectime; // in usec



void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0; //disable systick
	NVIC_ST_RELOAD_R = 79999999; //set load register to 10Hz (0.1 sec)
	NVIC_ST_CURRENT_R = 0; //reset current register
	NVIC_ST_CTRL_R = 7; //turn on the clock with interrupts enabled
}

void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x0E; //toggle for heartbeat
	ADCMail = ADC_In();
	ADCStatus = 1;
}

// your function to convert ADC sample to distance (0.01cm)
uint32_t Convert(uint32_t input){
	input = (201 * input)/4096 - 8;
  return input; // replace this line with your Lab 8 solution
}

int main(void){
  TExaS_Init();
  // your Lab 8
	PortF_Init();
	ADC_Init();
	ST7735_InitR(INITR_REDTAB); 
	SysTick_Init();
  EnableInterrupts();
  while(1){
		while(ADCStatus == 0){}; //wait for busy-wait mailbox flag to be true
		Data = ADCMail; //take ADC sample data
		ADCStatus = 0; //clear mailbox flag
		Position = Convert(Data); //convert data into a fixed-point numbeer using convert method
		ST7735_SetCursor(0,0); //Set cursor to print
		LCD_OutDec(Data);
		ST7735_OutString("  ");
		ST7735_SetCursor(6,0); //set cursor to print the fixed-point number
		LCD_OutFix(Position);
		ST7735_OutString(" cm");
  }
}

