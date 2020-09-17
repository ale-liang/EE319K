;****************** main.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Alex Liang
; Date Created: 1/15/2018 
; Last Modified: 2/4/2020 
; Brief description of the program: Solution to Lab1
; The objective of this system is to implement odd-bit counting system
; Hardware connections: 
;  Output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Inputs are negative logic, meaning switch not pressed is 1, pressed is 0
;    PE0 is an input 
;    PE1 is an input 
;    PE2 is an input 
;    PE3 is the output
; Overall goal: 
;   Make the output 1 if there is an odd number of switches pressed, 
;     otherwise make the output 0

; The specific operation of this system 
;   Initialize Port E to make PE0,PE1,PE2 inputs and PE3 an output
;   Over and over, read the inputs, calculate the result and set the output
; PE2  PE1 PE0  | PE3
; 0    0    0   |  1    odd number of 0’s
; 0    0    1   |  0    even number of 0’s
; 0    1    0   |  0    even number of 0’s
; 0    1    1   |  1    odd number of 0’s
; 1    0    0   |  0    even number of 0’s
; 1    0    1   |  1    odd number of 0’s
; 1    1    0   |  1    odd number of 0’s
; 1    1    1   |  0    even number of 0’s
;There are 8 valid output values for Port E: 0x01,0x02,0x04,0x07,0x08,0x0B,0x0D, and 0x0E. 

; NOTE: Do not use any conditional branches in your solution. 
;       We want you to think of the solution in terms of logical and shift operations

GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       THUMB
       AREA    DATA, ALIGN=2
;global variables go here
      ALIGN
      AREA    |.text|, CODE, READONLY, ALIGN=2
      EXPORT  Start
Start
    ;code to run once that initializes PE3,PE2,PE1,PE0
	LDR R0, =SYSCTL_RCGCGPIO_R		;load the clock into R0
	LDRB R1, [R0]					;take in the contents of clock into R1
	ORR R1, #0x10					;ORR the value of clock to turn it on
	STRB R1, [R0]					;store the changed value back into clock to turn it on
	NOP								;Wait for the clock to stabilize
	NOP
	LDR R0, = GPIO_PORTE_DIR_R		;Define the inputs and outputs by taking in each of pin locations
	LDRB R1, [R0]					;Load the contents of DIR of Port E into R0
	AND R1, #0xF8					;Make sure only Pins 0-3 are turned on
	ORR R1, #0x08					;Make PE3 1 to be certain
	STRB R1, [R0]
	LDR R0, =GPIO_PORTE_DEN_R		;Digitally enable pins by accessing the pins
	LDRB R1, [R0]					;Load the contents of Port E DEN into R1
	ORR R1, #0x0F					;Enable the pin bit on
	STRB R1, [R0]					;Store the R1 back into Port E DEN
	LDR R0, =GPIO_PORTE_DATA_R		;Load the data of Port E into R0
	
loop
	LDR R1, [R0]					;Load the contents of R0 into R1 (R1 meant to hold unaltered contents of R1
	AND R2, R1,	#0x01				;R2 will hold the bit for Pin 0 of Port E
	AND R3, R1, #0x02				;R3 will hold the bit for Pin 1 of Port E
	AND R4, R1, #0x04				;R4 will hold the bit for Pin 2 of Port E
	LSR R3, R3, #1					;Move the isolated bit of Pin 1 to least significant bit
	LSR R4, R4, #2					;Move the isolated bit of Pin 2 to least significant bit
	EOR R2, R2, R3					;Exclusive OR Pins 0-2 first to get the opposite of the expected result
	EOR R2, R2, R4
	EOR R2, R2, #0x01				;NOT the R1 to get the actual intended output
	LSL R2, R2, #3					;Shift the bit to bit 4
	MOV R1, R2						;Place contents of R2 into R1
	STR R1, [R0]					;write Port E, sets PE3
    B    loop


    
      ALIGN        ; make sure the end of this section is aligned
      END          ; end of file
          