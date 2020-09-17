;****************** main.s ***************
; Program written by: Zachary Chin and Alex Liang
; Date Created: 2/14/2017
; Last Modified: 3/3/2020
; You are given a simple stepper motor software system with one input and
; four outputs. This program runs, but you are asked to add minimally intrusive
; debugging instruments to verify it is running properly. 
; The system outputs in one of three modes:
; 1) cycles through 10,6,5,9,... at a constant rate
; 2) cycles through 5,6,10,9,... at a constant rate
; 3) does not change the output, but still outputs at a constant rate
; When PA4 goes high and low again, the system cycles through these modes
; The output rate will be different on the simulator versus the real board
;   Insert debugging instruments which gather data (state and timing)
;   to verify that the system is functioning as expected.
; Hardware connections (External: One button and four outputs to stepper motor)
;  PA4 is Button input  (1 means pressed, 0 means not pressed)
;  PE3-0 are stepper motor outputs 
;  PF2 is Blue LED on Launchpad used as a heartbeat
; Instrumentation data to be gathered is as follows:
; After every output to Port E, collect one state and time entry. 
; The state information is the 5 bits on Port A bit 4 and Port E PE3-0
;   place one 8-bit entry in your Data Buffer  
; The time information is the 24-bit time difference between this output and the previous (in 12.5ns units)
;   place one 32-bit entry in the Time Buffer
;    24-bit value of the SysTick's Current register (NVIC_ST_CURRENT_R)
;    you must handle the roll over as Current goes 3,2,1,0,0x00FFFFFF,0xFFFFFE,
; Note: The size of both buffers is 100 entries. Once you fill these
;       entries you should stop collecting data
; The heartbeat is an indicator of the running of the program. 
; On each iteration of the main loop of your program toggle the 
; LED to indicate that your code(system) is live (not stuck or dead).

SYSCTL_RCGCGPIO_R  EQU 0x400FE608
NVIC_ST_CURRENT_R  EQU 0xE000E018
GPIO_PORTA_DATA_R  EQU 0x400043FC
GPIO_PORTA_DIR_R   EQU 0x40004400
GPIO_PORTA_DEN_R   EQU 0x4000451C
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_DEN_R   EQU 0x4002551C
; RAM Area
          AREA    DATA, ALIGN=2
Index     SPACE 4 ; index into Stepper table 0,1,2,3
Direction SPACE 4 ; -1 for CCW, 0 for stop 1 for CW

;place your debug variables in RAM here
DataBuffer	  SPACE 100
TimeBuffer	  SPACE 400
DataPt	      SPACE 4
TimePt 		  SPACE 4
prevTime	  SPACE 3
count 	RN 4

; ROM Area
        IMPORT TExaS_Init
        IMPORT SysTick_Init
;-UUU-Import routine(s) from other assembly files (like SysTick.s) here
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
Stepper DCB 5,6,10,9
        EXPORT  Start

Start
 ; TExaS_Init sets bus clock at 80 MHz
; PA4, PE3-PE0 out logic analyzer to TExasDisplay
      LDR  R0,=SendDataToLogicAnalyzer
      ORR  R0,R0,#1
      BL   TExaS_Init ; logic analyzer, 80 MHz
 ;place your initializations here
      BL   Stepper_Init ; initialize stepper motor
      BL   Switch_Init  ; initialize switch input
	  BL   Heart_Init	; initialize heart beat output
;**********************
      BL   Debug_Init ;(you write this)
;**********************
      CPSIE  I    ; TExaS logic analyzer runs on interrupts
      MOV  R5,#0  ; last PA4
loop  
	  BL   Heart_Beat
      LDR  R1,=GPIO_PORTA_DATA_R
      LDR  R4,[R1]  ;current value of switch
      AND  R4,R4,#0x10 ; select just bit 4
      CMP  R4,#0
      BEQ  no     ; skip if not pushed
      CMP  R5,#0
      BNE  no     ; skip if pushed last time
      ; this time yes, last time no
      LDR  R1,=Direction
      LDR  R0,[R1]  ; current direction
      ADD  R0,R0,#1 ;-1,0,1 to 0,1,2
      CMP  R0,#2
      BNE  ok
      MOV  R0,#-1  ; cycles through values -1,0,1
ok    STR  R0,[R1] ; Direction=0 (CW)  
no    MOV  R5,R4   ; setup for next time
      BL   Stepper_Step               
      LDR  R0,=1600000
      BL   Wait  ; time delay fixed but not accurate   
      B    loop
;Initialize stepper motor interface
Stepper_Init
      MOV R0,#1
      LDR R1,=Direction
      STR R0,[R1] ; Direction=0 (CW)
      MOV R0,#0
      LDR R1,=Index
      STR R0,[R1] ; Index=0
    ; activate clock for Port E
      LDR R1, =SYSCTL_RCGCGPIO_R
      LDR R0, [R1]
      ORR R0, R0, #0x10  ; Clock for E
      STR R0, [R1]
      NOP
      NOP                 ; allow time to finish activating
    ; set direction register
      LDR R1, =GPIO_PORTE_DIR_R
      LDR R0, [R1]
      ORR R0, R0, #0x0F    ; Output on PE0-PE3
       STR R0, [R1]
    ; enable digital port
      LDR R1, =GPIO_PORTE_DEN_R
      LDR R0, [R1]
      ORR R0, R0, #0x0F    ; enable PE3-0
      STR R0, [R1]
      BX  LR
	  
;Initialize switch interface
Switch_Init
    ; activate clock for Port A
      LDR R1, =SYSCTL_RCGCGPIO_R
      LDR R0, [R1]
      ORR R0, R0, #0x01  ; Clock for A
      STR R0, [R1]
      NOP
      NOP                 ; allow time to finish activating
    ; set direction register
      LDR R1, =GPIO_PORTA_DIR_R
      LDR R0, [R1]
      BIC R0, R0, #0x10    ; Input on PA4
      STR R0, [R1]
    ; 5) enable digital port
      LDR R1, =GPIO_PORTA_DEN_R
      LDR R0, [R1]
      ORR R0, R0, #0x10    ; enable PA4
      STR R0, [R1]
      BX  LR
	  
;Initialize heart beat in PF2
Heart_Init
	  ;Activate clock for Port F
	  LDR R1, =SYSCTL_RCGCGPIO_R
      LDR R0, [R1]
      ORR R0, R0, #0x20  ; Clock for F
      STR R0, [R1]
      NOP
      NOP                 ; allow time to finish activating
    ; set direction register
      LDR R1, =GPIO_PORTF_DIR_R
      LDR R0, [R1]
      ORR R0, R0, #0x04    ; Output on PF2
      STR R0, [R1]
    ; 5) enable digital port
      LDR R1, =GPIO_PORTF_DEN_R
      LDR R0, [R1]
      ORR R0, R0, #0x04    ; enable PF2
      STR R0, [R1]
      BX  LR
	  
; Step the motor clockwise
; Direction determines the rotational direction
; Input: None
; Output: None
Stepper_Step
      PUSH {R4,LR}
      LDR  R1,=Index
      LDR  R2,[R1]     ; old Index
      LDR  R3,=Direction
      LDR  R0,[R3]     ; -1 for CCW, 0 for stop 1 for CW
      ADD  R2,R2,R0
      AND  R2,R2,#3    ; 0,1,2,3,0,1,2,...
      STR  R2,[R1]     ; new Index
      LDR  R3,=Stepper ; table
      LDRB R0,[R2,R3]  ; next output: 5,6,10,9,5,6,10,...
      LDR  R1,=GPIO_PORTE_DATA_R ; change PE3-PE0
      STR  R0,[R1]
      BL   Debug_Capture
      POP {R4,PC}
	  
; inaccurate and inefficient time delay
Wait 
      SUBS R0,R0,#1  ; outer loop
      BNE  Wait
      BX   LR

; subroutine to toggle Port F pin 2
; input: none
; output: none
Heart_Beat
	  PUSH {R0-R1}					;Save registers used
	  LDR R0, =GPIO_PORTF_DATA_R	;Grab the Port F data registers
	  LDR R1, [R0]
	  EOR R1, #0x04					;Toggle pin 2 and store it back
	  STR R1, [R0]
	  POP  {R0-R1}					;Restore registers
	  BX LR

; Initializes the debugger
; All registers saved and restored within
; Input: None
; Output: None
Debug_Init 
      PUSH {R0-R4,LR}			;Save registers R0-R4 and LR to stack
; you write this
	  LDR R2, =DataBuffer		;R2 points to the start of the Data Array
	  LDR R3, =TimeBuffer		;R3 points to the start of the Time Array
	  MOV count, #100 			;count (R4) keeps track of num spots instantiated
	  MOV R1, #-1				;R1 holds 0xFF/0xFFFFFFFF to fill the arrays with
deLoop							;This loop runs 100 times to fill all 100 spots in both arrays	
	  STRB R1, [R2]				;Store 0xFF in the next spot in the data buffer
	  STR R1, [R3]				;Store 0xFFFFFFFF in the next spot in the time buffer
	  ADD R2, #1				;Update R2 to point to the next element (1 byte away)
	  ADD R3, #4				;Update R3 to point to the next element (4 bytes away)
	  SUBS count, #1			;Decrement count by 1
	  BNE deLoop				;Check if we've reached iteration #100. Loop back if not
	  LDR R2, =DataBuffer		;R2 points back to the start of the Data Array
	  LDR R3, =TimeBuffer		;R3 points back to the start of the Time Array
	  LDR R0, =DataPt			;R0 grabs the location DataPt	
	  LDR R1, =TimePt			;R1 grabs the location TimePt
	  STR R2, [R0]				;Store the start of the Data Array in DataPt
	  STR R3, [R1]				;Store the start of the Time Array in TimePt
	  LDR R0, =prevTime			;Instantiate the previous capture time to 0x00FFFFFF
	  MOV R1, #0
	  ORR R1, #0x00FFFFFF
	  STR R1, [R0]				;Store it back into prevTime
	  BL  SysTick_Init			;Branch to initialize the SysTick
	
      POP {R0-R4,PC}			;Reload registers pushed onto the stack
	  ;Don't need BX LR because we are popping the LR into the PC
	
; Captures each data point and adds data and time to their respective arrays
; All registers saved and restored within
; 30 Instructions, so ~60 cycles and 750ns
; In between Debug Captures: ~6,400,081 cycles and ~80,001,012.5ns
; Percentage of time to run debug capture = ~0.0000937%
; Input: None
; Output: None   
Debug_Capture 	
      PUSH {R0-R6,LR}				;Save registers R0-R6 and LR to stack
; you write this
	  LDR R3, =DataPt				;R3 grabs the position of DataPt	
	  LDR R5, [R3]					;R5 holds the position of the next element added to Data Buffer
	  LDR R4, =DataBuffer			;R4 grabs the start of the Data Buffer
	  ADD R4, #100					;Increment R4 by 100 to find the location directly after the buffer ends
	  CMP R5, R4					;Check if the buffer is full (next element is past the end of the buffer)
	  BEQ ds_done					;If full, branch to the end
	  LDR R0, =GPIO_PORTA_DATA_R	;R0 grabs Port A's Data register
	  LDRB R1, [R0]					;R1 grabs the contents of the Data register
	  AND R1, R1, #0x10				;Mask the 4th bit to grab the value of PA4
	  LDR R0, =GPIO_PORTE_DATA_R	;R0 grabs Port E's Data register
	  LDRB R4, [R0]					;R4 grabs the contents of the Data register
	  AND R4, R4, #0xF				;Mask bits 0-3 to grab the value of PE0-PE3
	  ORR R4, R1, R4				;Or the two values together into one 5-bit value (5, 6, A, 9)
	  STRB R4, [R5]					;Store that value into the next spot in Data Buffer
	  ADD R5, #1					;Increment the DataPt so it points to the new next position
	  STR R5, [R3]					;Store it into DataPt
	  
	  LDR R3, =TimePt				;R3 grabs the position of TimePt
	  LDR R5, [R3]					;R5 grabs the position of the next element added to Time Buffer
	  LDR R1, =NVIC_ST_CURRENT_R	;R1 points to the systick current register
	  LDR R1, [R1]					;Grab the current value of the systick current register
	  LDR R2, =prevTime				;R2 points to the storage point of the previous time 
	  LDR R0, [R2]					;R0 holds the previous time (0xFFFFFF if first time)
	  BL sub24						;Calls sub24 to do 24-bit subtraction (prev - current)
	  STR R1, [R2]					;Store the current time into prevTime
	  STR R0, [R5]					;Stores the difference into the Time Buffer
	  ADD R5, #4					;Increment TimePt by 4 (to the next 32-bit element)
	  STR R5, [R3]					;Store it back into TimePt
	  
ds_done
	  POP  {R0-R6,PC}				;Restore the registers
	  BX LR				

; Takes 2 inputs (R0 and R1) and subtracts them
; Returns the difference
sub24
      PUSH {R1, R2}
	  CMP R0, R1
	  BHS normal
	  MOV R2, #0
	  ORR R2, #0x01000000
	  SUB R1, R0
	  SUB R0, R2, R1
	  B exit24
normal	
	  SUB R0, R0, R1
exit24
      POP {R1, R2}
	  BX LR
	  
; edit the following only if you need to move pins from PA4, PE3-0      
; logic analyzer on the real board
PA4  equ  0x40004040   ; bit-specific addressing
PE30 equ  0x4002403C   ; bit-specific addressing
UART0_DR_R equ 0x4000C000 ;write to this to send data
SendDataToLogicAnalyzer
     LDR  R1,=PA4  
     LDR  R1,[R1]  ; read PA4
     LDR  R0,=PE30 ; read PE3-PE0
     LDR  R0,[R0]
     ORR  R0,R0,R1 ;combine into one 5-bit value
     ORR  R0,R0,#0x80 ;bit7=1 means digital data
     LDR  R1,=UART0_DR_R
     STR  R0,[R1] ; send data at 10 kHz
     BX   LR


     ALIGN    ; make sure the end of this section is aligned
     END      ; end of file
