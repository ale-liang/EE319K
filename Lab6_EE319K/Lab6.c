// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"
#include "music.h"

#define C1   1194   // 2093 Hz
#define B1   1265   // 1975.5 Hz
#define BF1  1341   // 1864.7 Hz
#define A1   1420   // 1760 Hz
#define AF1  1505   // 1661.2 Hz
#define G1   1594   // 1568 Hz
#define GF1  1689   // 1480 Hz
#define F1   1790   // 1396.9 Hz
#define E1   1896   // 1318.5 Hz
#define EF1  2009   // 1244.5 Hz
#define D1   2128   // 1174.7 Hz
#define DF1  2255   // 1108.7 Hz
#define C    2389   // 1046.5 Hz
#define B    2531   // 987.8 Hz
#define BF   2681   // 932.3 Hz
#define A    2841   // 880 Hz
#define AF   3010   // 830.6 Hz
#define G    3189   // 784 Hz
#define GF  3378   // 740 Hz
#define F   3579   // 698.5 Hz
#define E   3792   // 659.3 Hz
#define EF  4018   // 622.3 Hz
#define D   4257   // 587.3 Hz
#define DF  4510   // 554.4 Hz
#define C0  4778   // 523.3 Hz
#define B0  5062   // 493.9 Hz
#define BF0 5363   // 466.2 Hz
#define A0  5682   // 440 Hz
#define AF0 6020   // 415.3 Hz
#define G0  6378   // 392 Hz
#define GF0 6757   // 370 Hz
#define F0  7159   // 349.2 Hz
#define E0  7584   // 329.6 Hz
#define EF0 8035   // 311.1 Hz
#define D0  8513   // 293.7 Hz
#define DF0 9019   // 277.2 Hz
#define C7  9556   // 261.6 Hz
#define B7  10124   // 246.9 Hz
#define BF7 10726   // 233.1 Hz
#define A7  11364   // 220 Hz
#define AF7 12039   // 207.7 Hz
#define G7  12755   // 196 Hz
#define GF7 13514   // 185 Hz
#define F7  14317   // 174.6 Hz
#define E7  15169   // 164.8 Hz
#define EF7 16071   // 155.6 Hz
#define D7  17026   // 146.8 Hz
#define DF7 18039   // 138.6 Hz
#define C6  19111   // 130.8 Hz


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


int main(void){
	DisableInterrupts();
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init();
	Music_Init();
	uint8_t previous = 0;
  // other initialization
  EnableInterrupts();
  while(1){
		uint8_t buttPress;
		buttPress = Piano_In();
		if(buttPress != previous){
			
			if(buttPress == 0x01){
				//Sound_Play(A0);
				Music_Play();
			}
			else if(buttPress == 0x02){
				//Sound_Play(B0);
				Music_Stop();
			}
			else if(buttPress == 0x04){
				Sound_Play(G0);
			}
			else if(buttPress == 0){
				Sound_Play(0);
			}
			previous = buttPress;
		}
  }    
} 

/*
void Delay(void){
	int r0 = 0;
	__asm{    
		and r0,r0  // 12.5ns each
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
		and r0,r0
	}
}
// very very approximate delay
void Delay1ms(int time){ int i;
  for(;time;time--){
    for(i=0;i<1200;i++) Delay();
  }
}

//---------------------LaunchPad_Init---------------------
// initialize switch interface
// Input: none
// Output: none
void LaunchPad_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020; 	// activate clock for Port F;
  delay = 100; delay=100; delay=100;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;       	// allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_DIR_R = 0x0E;      	// PF4,PF0 in, PF3-1 out
  GPIO_PORTF_PUR_R = 0x11;      	// enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;      	// enable digital I/O on PF4-0
}

//---------------------Switch_In---------------------
// read the values of the two switches
// Input: none
// Output: 0x00,0x01,0x10,0x11 from the two switches
//     	0 if no switch is pressed
// bit4 PF4 SW1 switch
// bit0 PF0 SW2 switch
uint32_t Switch_In(void){
  return (GPIO_PORTF_DATA_R&0x11)^0x11;
}


// lab video Lab6_static
int main(void){   uint32_t last,now;  
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);	// bus clock at 80 MHz
  LaunchPad_Init();
  DAC_Init(); // your lab 6 solution
  uint32_t Testdata = 7;
  EnableInterrupts();
  last = Switch_In();
  while(1){           	 
    now = Switch_In();
    if((last != now)&&now){
      Testdata = (Testdata+1)&0x07;
      DAC_Out(Testdata); // your lab 6 solution
    }
    last = now;
    Delay1ms(25);   // debounces switch
  }
}

*/


