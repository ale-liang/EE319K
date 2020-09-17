//Alex Liang and Zachary Chin
//all3344, zhc94
//StepperMotorController.c starter file EE319K Lab 5
// Runs on TM4C123
// Finite state machine to operate a stepper motor.  
// Jonathan Valvano
// 1/17/2020

// Hardware connections (External: two input buttons and four outputs to stepper motor)
//  PA5 is Wash input  (1 means pressed, 0 means not pressed)
//  PA4 is Wiper input  (1 means pressed, 0 means not pressed)
//  PE5 is Water pump output (toggle means washing)
//  PE4-0 are stepper motor outputs 
//  PF1 PF2 or PF3 control the LED on Launchpad used as a heartbeat
//  PB6 is LED output (1 activates external LED on protoboard)

#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define Off 0										//This is the initial state where neither wiper or LED is on
#define S1NoL1 1								//The S stands for step, as in which bit step the motor is on, and the NoL stands for no LED being on
#define S2NoL1 2								//The last number of the name stands for which cycle the motor is currently on
#define	S4NoL1 3
#define S8NoL1 4
#define S16NoL1 5
#define S1NoL2 6
#define S2NoL2 7
#define S4NoL2 8
#define S8NoL2 9
#define S16NoL2 10
#define S8NoL3 11
#define S4NoL3 12
#define S2NoL3 13
#define S1NoL3 14
#define S16NoL4 15
#define S8NoL4 16
#define S4NoL4 17
#define S2NoL4 18
#define S1NoL4 19
#define S1L1 20								//The L stands for the LED being on in this state
#define S4L1 21
#define S16L1 22
#define S2L2 23
#define S8L2 24
#define	S8L3 25
#define S2L3 26
#define S16L4 27
#define S4L4 28
#define S1L4 29

struct state{					
	uint8_t Next[4];						//Next[4] represents the array of states that the motor can proceed into based on the input
	uint8_t output;							//output represent the current state of the StepperMotor's output (meaning which position the motor should be at)
	uint32_t wait;							//wait represents the time in ms of the delay needed after each step has been completed
};

void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0      
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))
void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

typedef struct state state_t; //Need to typedef struct state state_t so that state_t can always be referred to without needing the struct before it

	state_t fsm[30] = {																//Implements the FSM with 30 states, with each output and wait time in ms after each step
		{{Off, S1NoL1, S1L1, S1L1}, 0x0, 50},										
		{{S2NoL1, S2NoL1, S2NoL1, S2NoL1}, 0x1, 50},
		{{S4NoL1, S4NoL1, S4L1, S4L1}, 0x2, 50},
		{{S8NoL1, S8NoL1, S8NoL1, S8NoL1}, 0x4, 50},
		{{S16NoL1, S16NoL1, S16L1, S16L1}, 0x8, 50},
		{{S1NoL2, S1NoL2, S1NoL2, S1NoL2}, 0x10, 50},
		{{S2NoL2, S2NoL2, S2L2, S2L2}, 0x1, 50},
		{{S4NoL2, S4NoL2, S4NoL2, S4NoL2}, 0x2, 50},
		{{S8NoL2, S8NoL2, S8L2, S8L2}, 0x4, 50},
		{{S16NoL2, S16NoL2, S16NoL2, S16NoL2}, 0x8, 50},
		{{S8NoL3, S8NoL3, S8L3, S8L3}, 0x10, 50},
		{{S4NoL3, S4NoL3, S4NoL3, S4NoL3}, 0x8, 50},
		{{S2NoL3, S2NoL3, S2L3, S2L3}, 0x4, 50},
		{{S1NoL3, S1NoL3, S1NoL3, S1NoL3}, 0x2, 50},
		{{S16NoL4, S16NoL4, S16L4, S16L4}, 0x1, 50},
		{{S8NoL4, S8NoL4, S8NoL4, S8NoL4}, 0x10, 50},
		{{S4NoL4, S4NoL4, S4L4, S4L4}, 0x8, 50},
		{{S2NoL4, S2NoL4, S2NoL4, S2NoL4}, 0x4, 50},
		{{S1NoL4, S1NoL4, S1L4, S1L4}, 0x2, 50},
		{{Off, Off, Off, Off}, 0x1, 50},
		{{S2NoL1, S2NoL1, S2NoL1, S2NoL1}, 0x21, 50},
		{{S8NoL1, S8NoL1, S8NoL1, S8NoL1}, 0x24, 50},
		{{S1NoL2, S1NoL2, S1NoL2, S1NoL2}, 0x30, 50},
		{{S4NoL2, S4NoL2, S4NoL2, S4NoL2}, 0x22, 50},
		{{S16NoL2, S16NoL2, S16NoL2, S16NoL2}, 0x28, 50},
		{{S4NoL3, S4NoL3, S4NoL3, S4NoL3}, 0x28, 50},
		{{S1NoL3, S1NoL3, S1NoL3, S1NoL3}, 0x22, 50},
		{{S8NoL4, S8NoL4, S8NoL4, S8NoL4}, 0x30, 50},
		{{S2NoL4, S2NoL4, S2NoL4, S2NoL4}, 0x24, 50},
		{{Off, Off, Off, Off}, 0x21, 50},
	};

int main(void){ 
	volatile uint8_t nop;										//wait for system clock to stabilize (initialize nop as a variable)
	uint8_t cs;	
	uint8_t in;
  TExaS_Init(&SendDataToLogicAnalyzer);   // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();   
// you initialize your system here
	SYSCTL_RCGCGPIO_R |= 0x33;							//Turn on clocks for Port A, B, E, and F
	nop++;																	//Wait for system to stabilize
	nop++;
	GPIO_PORTA_DIR_R &= 0xCF;								//Make pins for Port A 4 and 5 inputs
	GPIO_PORTA_DEN_R |= 0x30;								//Digitally enable pins 4 and 5
	GPIO_PORTE_DIR_R |= 0x3F;								//Make pins for Port E 0-5 outputs
	GPIO_PORTE_DEN_R |= 0x3F;								//Digitally enable pins 0-5 
	GPIO_PORTB_DIR_R |= 0x40;								//Make pin 6 for Port B an output
	GPIO_PORTB_DEN_R |= 0x40;								//Digitally enable pin 6
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;			//Unlock Port F
	GPIO_PORTF_CR_R |= 0xFF;				
	GPIO_PORTF_DIR_R |= 0x2;								//Make pin 2 for Port F an output
	GPIO_PORTF_DEN_R |= 0x2;								//Digitally enable pin 2
	//GPIO_PORTF_PDR_R |= 0x2;							//Enable the Pull down resistor
	
  EnableInterrupts();   
	
	cs = Off;																//set initial state of cs to be the Off state
  while(1){
// output
// wait
// input
// next		
	  GPIO_PORTE_DATA_R = fsm[cs].output;		//Put the output value of the current state of the FSM into Port E data
	  SysTick_Wait1ms(fsm[cs].wait);				//Wait the specific amount of time in ms given by the current state of the FSM
		in = (GPIO_PORTA_DATA_R & 0x30) >> 4;	//in is the input and takes the data from Port A and places it into the FSM
		cs = fsm[cs].Next[in];								//cs is the current state and uses that to take the input from Port A to decide the next state
		GPIO_PORTF_DATA_R ^= 0x2;							//Toggle pin 2 of Port F to create the heartbeat
  }
}
