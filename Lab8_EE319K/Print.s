; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
		
number EQU 0
counter EQU 4

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
    ; copy/paste Lab 7 solution here
	PUSH {R0,R4-R11, LR}
	SUB SP, #8	;Allocation of stack
	MOV R11, SP
	CMP R0, #10 		;check if R0 only holds a single digit
	BLO printOutSingle
	MOV R7, R0			;Count number of digits in input (place original number in R7)
	MOV R6, #0x0A		;set R6 = 10
	MOV R8, #0			;Make R8 a counter (initialized as 1)
countDigLoop	
	ADD R8, R8, #1
	UDIV R7, R7, R6		;Divide input by 10 to count number of digits
	CMP R7, #0
	BNE countDigLoop
	MOV R7, R0				;Reset R7 as original input value
nextDigitStackLoop
	UDIV R7, R7, R6			;take away the least significant digit (if R7/10 = 0, it means no more digits to print)
	MUL R7, R7, R6			;R7 = (R0/10) * 10 => R0 = *** and R7 = **0
	SUB R0, R0, R7			;=> R0 = 00*
	PUSH {R0, LR}			;push onto stack
	UDIV R7, R7, R6			;precede to next digit
	CMP R7, #0
	BEQ printOutStack
	MOV R0, R7				;R0 without its least significant digit
	B nextDigitStackLoop
printOutStack
	STR R8, [R11, #counter]	;local variable number will just hold number of digits in number to print
stackLoop
	LDR R8, [R11, #counter]
	CMP R8, #0
	BEQ finish
	SUB R8, #1
	STR R8, [R11, #counter]
	POP {R0, LR}
	ADD R0, #0x30
	BL ST7735_OutChar
	B stackLoop
printOutSingle
	ADD R0, #0x30
	BL ST7735_OutChar
finish
	ADD SP, #8	;Deallocation of stack
	POP {R0,R4-R11, LR}

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
    ; copy/paste Lab 7 solution here
	PUSH {R0,R4-R11, LR}
	SUB SP, #8		;Allocate space for variables
	MOV R6, #999
	CMP R0, R6
	BHI printStars	;exception case when R0>999, print "*.**"
	MOV R6, #100	;want to print out the most significant digit
	MOV R4, R0		;place original value into R4
	STR R4, [SP, #number]
	UDIV R0, R0, R6 ;have R0 contain only the most significant digit
	ADD R0, #0x30
	BL ST7735_OutChar	;print most significant digit
	MOV R0, #0x2E
	BL ST7735_OutChar	;print out the decimal
	LDR R0, [SP, #number]		;Have R0 return to original value
	UDIV R4, R4, R6 ;Divide by 100 so input only has most significant digit
	MUL R4, R4, R6	;Multiply R4 by 100 so it can have same degree as R0
	SUB R0, R0, R4	;Subtract R4 from R0 so now it has the last 2 digits
	MOV R6, #10		;want to print out the 2nd digit
	MOV R4, R0		;keep the current value in R1
	STR R4, [SP, #number]
	UDIV R0, R0, R6	;place 2nd digit only into R0
	ADD R0, #0x30
	BL ST7735_OutChar	;print 2nd digit
	LDR R0, [SP, #number]
	UDIV R0, R0, R6
	MUL R0, R0, R6
	SUB R4, R4, R0
	MOV R0, R4
	ADD R0, #0x30
	BL ST7735_OutChar
	B finishOutFix
printStars
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
finishOutFix

	ADD SP, #8 		;Deallocate space for local variables
	POP {R0,R4-R11, LR}
     BX   LR
 
     
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
