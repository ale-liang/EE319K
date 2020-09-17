; SysTick.s
; Module written by: Zachary Chin and Alex Liang
; Date Created: 2/14/2017
; Last Modified: 3/3/2020 
; Brief Description: Initializes SysTick

NVIC_ST_CTRL_R        EQU 0xE000E010
NVIC_ST_RELOAD_R      EQU 0xE000E014
NVIC_ST_CURRENT_R     EQU 0xE000E018

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
; -UUU- You add code here to export your routine(s) from SysTick.s to main.s
        EXPORT SysTick_Init
;------------SysTick_Init------------
; ;-UUU-Complete this subroutine
; Initialize SysTick running at bus clock.
; Make it so NVIC_ST_CURRENT_R can be used as a 24-bit time
; Input: none
; Output: none
; Modifies: ??
SysTick_Init
 ; **-UUU-**Implement this function****
	LDR R0, =NVIC_ST_CTRL_R			;Set the CTRL Register to 0 (turn it off)
	MOV R1, #0
	STR R1, [R0]
	LDR R0, =NVIC_ST_RELOAD_R		;Set the Reload Register to 0xFFFFFF (max value for 24-bit timer)
	ORR R1, #0x00FFFFFF
	STR R1, [R0]
	LDR R0, =NVIC_ST_CURRENT_R		;Reset the Current Register
	MOV R1, #0
	STR R1, [R0]
	LDR R0, =NVIC_ST_CTRL_R			;Enable the CTRL register and the Clock Source
	MOV R1, #5
	STR R1, [R0]
    BX  LR                          ; return


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file