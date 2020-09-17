// StepperMotorController.c starter file EE319K Lab 5
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

#define Off 0
#define S1NoL1 1
#define S2NoL1 2
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
#define S1L1 20
#define S2L1 21
#define S4L1 22
#define S8L1 23
#define S16L1 24
#define	S8L3 25
#define S4L3 26
#define S2L3 27
#define S1L3 28

struct state{
	uint8_t Next[4];
	uint8_t output;
	uint32_t wait;
};

void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0      
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))
void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

typedef struct state state_t;

	state_t fsm[29] = {
		{{Off, S1NoL1, S1L1, S1L1}, 0x0, 50},
		{{S2NoL1, S2NoL1, S2NoL1, S2NoL1}, 0x1, 50},
		{{S4NoL1, S4NoL1, S4NoL1, S4NoL1}, 0x2, 50},
		{{S8NoL1, S8NoL1, S8NoL1, S8NoL1}, 0x4, 50},
		{{S16NoL1, S16NoL1, S16NoL1, S16NoL1}, 0x8, 50},
		{{S1NoL2, S1NoL2, S1NoL2, S1NoL2}, 0x10, 50},
		{{S2NoL2, S2NoL2, S2NoL2, S2NoL2}, 0x1, 50},
		{{S4NoL2, S4NoL2, S4NoL2, S4NoL2}, 0x2, 50},
		{{S8NoL2, S8NoL2, S8NoL2, S8NoL2}, 0x4, 50},
		{{S16NoL2, S16NoL2, S16NoL2, S16NoL2}, 0x8, 50},
		{{S8NoL3, S8NoL3, S8L3, S8L3}, 0x10, 50},
		{{S4NoL3, S4NoL3, S4NoL3, S4NoL3}, 0x8, 50},
		{{S2NoL3, S2NoL3, S2NoL3, S2NoL3}, 0x4, 50},
		{{S1NoL3, S1NoL3, S1NoL3, S1NoL3}, 0x2, 50},
		{{S16NoL4, S16NoL4, S16NoL4, S16NoL4}, 0x1, 50},
		{{S8NoL4, S8NoL4, S8NoL4, S8NoL4}, 0x10, 50},
		{{S4NoL4, S4NoL4, S4NoL4, S4NoL4}, 0x8, 50},
		{{S2NoL4, S2NoL4, S2NoL4, S2NoL4}, 0x4, 50},
		{{S1NoL4, S1NoL4, S1NoL4, S1NoL4}, 0x2, 50},
		{{Off, Off, Off, Off}, 0x1, 50},
		{{S2L1, S2L1, S2L1, S2L1}, 0x21, 50},
		{{S4L1, S4L1, S4L1, S4L1}, 0x22, 50},
		{{S8L1, S8L1, S8L1, S8L1}, 0x24, 50},
		{{S16L1, S16L1, S16L1, S16L1}, 0x28, 50},
		{{S1NoL2, S1NoL2, S1NoL2, S1NoL2}, 0x30, 50},
		{{S4L3, S4L3, S4L3, S4L3}, 0x28, 50},
		{{S2L3, S2L3, S2L3, S2L3}, 0x24, 50},
		{{S1L3, S1L3, S1L3, S1L3}, 0x22, 50},
		{{S16NoL4, S16NoL4, S16NoL4, S16NoL4}, 0x21, 50},
	};

int main(void){ 
	volatile uint8_t nop;
	uint8_t cs;
	uint8_t in;
  TExaS_Init(&SendDataToLogicAnalyzer);    // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();   
// you initialize your system here
	SYSCTL_RCGCGPIO_R |= 0x33;
	nop++;
	nop++;
	GPIO_PORTA_DIR_R &= 0xCF;
	GPIO_PORTA_DEN_R |= 0x30;
	GPIO_PORTE_DIR_R |= 0x3F;
	GPIO_PORTE_DEN_R |= 0x3F;
	GPIO_PORTB_DIR_R |= 0x40;
	GPIO_PORTB_DEN_R |= 0x40;
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0xFF;
	GPIO_PORTF_DIR_R &= 0xFFFD;
	GPIO_PORTF_DEN_R |= 0x2;
	
  EnableInterrupts();   
	
	cs = Off;
  while(1){
// output
// wait
// input
// next		
	  GPIO_PORTE_DATA_R = fsm[cs].output;
	  SysTick_Wait1ms(fsm[cs].wait);
		in = (GPIO_PORTA_DATA_R & 0x30) >> 4;
		cs = fsm[cs].Next[in];
  }
}


