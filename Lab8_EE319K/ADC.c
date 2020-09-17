// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/17/2020
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ volatile int delay;
	
	SYSCTL_RCGCGPIO_R |= 0x08; //activate clock for Port D
	SYSCTL_RCGCADC_R |= 0x0001; //activate ADC0
	delay = SYSCTL_RCGCGPIO_R;
	delay++;
	delay++;
	delay++;
	delay++;
	GPIO_PORTD_DIR_R &= ~0x04; //make PD2 an input
	GPIO_PORTD_AFSEL_R |= 0x04; //enable alternate function on PD2
	GPIO_PORTD_DEN_R &= ~0x04; //disable digital interface for PD2
	GPIO_PORTD_AMSEL_R |= 0x04; //enable analog functionality for PD2
	ADC0_PC_R = 0x01; 	//configure for 125K
	ADC0_SSPRI_R = 0x123; //set Seq 3 highest priority
	ADC0_ACTSS_R &= ~0x0008; //disable seq 3
	ADC0_EMUX_R &= ~0xF000; //set seq3 as the software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 5; //set channel number as 5 so Ain5
	ADC0_SSCTL3_R = 0x0006;		//no TS0D0, yes IE0END0
	ADC0_IM_R &= ~0x0008;         // disable SS3 interrupts
	ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
	ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  uint32_t result;
	ADC0_PSSI_R = 0x0008; //start SS3
	while((ADC0_RIS_R&0x08) == 0){}; //wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF; //read result
	ADC0_ISC_R = 0x0008; //acknowledge completion and clear ADC0_RIS-R bit3
		
  return result; // remove this, replace with real code
}


