//music.c

#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "music.h"
#include <stdlib.h>
#include "sound.h"

#define bus 80000000
#define quarter (bus/4)
#define quarterP (bus/(8/3))
#define half (bus/2)
#define whole (bus)
#define eighth (bus/8)
#define eighthP (bus/(16/3))
#define sixteenth (bus/16)
#define gap (bus/40)

#define noS 0			//no sound
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

struct note{
	uint32_t frequency;
	uint32_t length;
	uint32_t volume;
};

typedef const struct note note_t;

note_t *songPt; //points to note as song plays
uint32_t pitch;
uint32_t duration = 1;
uint32_t vol;

note_t HappyBirthday[50] = {
	{G0, eighthP, 1},
	{noS, gap, 1},
	{G0, eighth, 1},
	{noS, gap, 1},
	{A0, quarter, 1},
	{noS, gap, 1},
	{G0, quarter, 1},
	{noS, gap, 1},
	{C0, quarter, 2},
	{noS, gap, 1},
	{B0, half, 4},
	{G0, eighthP, 1},
	{noS, gap, 1},
	{G0, eighth, 1},
	{noS, gap, 1},
	{A0, quarter, 1},
	{noS, gap, 1},
	{G0, quarter, 1},
	{noS, gap, 1},
	{D, quarter, 2},
	{noS, gap, 1},
	{C0, half, 4},
	{noS, gap, 1},
	{G0, eighthP, 1},
	{noS, gap, 1},
	{G0, eighth, 1},
	{noS, gap, 1},
	{G1, quarter, 1},
	{noS, gap, 1},
	{E, quarter, 2},
	{noS, gap, 1},
	{C0, quarter, 4},
	{noS, gap, 1},
	{B0, quarter, 8},
	{noS, gap, 1},
	{A0, quarter, 8},
	{noS, gap, 1},
	{F, eighthP, 16},
	{noS, gap, 1},
	{F, eighth, 16},
	{noS, gap, 1},
	{E, quarter, 16},
	{noS, gap, 1},
	{C0, quarter, 16},
	{noS, gap, 1},
	{D, quarter, 16},
	{noS, gap, 1},
	{C0, half, 16},
	{0, 0, 0},
}; 
//turns on TIMER0 interrupts to run user task
//inputs: task is a pointer to a user function period in units (1/clockfreq), 32 bitrs
//Outputs: none
void Timer0A_Init(uint32_t period){
	volatile int delay;
	SYSCTL_RCGCTIMER_R |= 0x01; // activate TIMER0
	delay++;
	TIMER0_CTL_R = 0x00000000;	//disable TIMER0A
	TIMER0_CFG_R = 0x00000000;	//configure timer for 32 bits
	TIMER0_TAMR_R = 0x00000002; //configure for periodic interrupts
	TIMER0_TAILR_R = period-1; 	//reload value  (TAILR)
	TIMER0_TAPR_R = 0;					//bus clock resolution
	TIMER0_ICR_R = 0x00000001;		//clear TIMER0A timer
	TIMER0_IMR_R = 0x00000000;		//arm timeout interrupt arms it
	NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF) | 0x80000000; //becomes priority 4 assigns priority
	//interrupts enabled in the main program
	//vector number 35, intterupt number 19
	NVIC_EN0_R = 1<<19;					//enables it
	TIMER0_CTL_R = 0x00000001; 	//turn on the timer
}

void Timer0A_Handler(void){ //called every time want to change the note
	TIMER0_ICR_R = TIMER_ICR_TATOCINT; //acknowledge timer and will clear the interrupt flag	
	//disarm and quit the ISR if 
	//	songPointer is null 
	//	or songPointer is at the end
	//else
	//	Sound_Play(songPt -> frequency);
	// TIMER0_TAILR_R = songPt -> length;
	// increment songPt
	if((duration == 0) || (songPt == NULL)){
		TIMER0_IMR_R = 0x00000000; //disarm Timer0A
		TIMER0_CTL_R = 0x00000000; // deactivate TIMER0
	}
	else{
		pitch = songPt->frequency;
		duration = songPt->length;
		vol = songPt->volume;
		Sound_Play(pitch);
		TIMER0_TAILR_R = (duration);
		if(vol > 0){
			Envelope();
		}
		songPt++;
		
	}
}



void Music_Init(void){
	//initialize Timer0A, Timer0A_Init
	Timer0A_Init(10);
	TIMER0_IMR_R = 0x00000000; //disarm Timer0A
	songPt = NULL;
	//set songPointer to null
}

void Music_Play(void){
	//initialize songPointer
	songPt = &HappyBirthday[0];
	TIMER0_IMR_R = 0x00000001;	//ARM timer0A
}

void Music_Stop(void){
	TIMER0_IMR_R = 0x00000000; //disarm Timer0A
	songPt = NULL;	
}
