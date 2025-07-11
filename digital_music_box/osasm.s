;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123/MSP432
; Lab 2 starter file
; February 10, 2016
;


        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  StartOS
        EXPORT  SysTick_Handler
        IMPORT  Scheduler

; Code copied from lecture slide 63 -Jonathan Arc
SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
	PUSH	{R4-R11}		   ; 3) Save remaining regs r4-11
	LDR		R0, =RunPt		   ; 4) R0=pointer to RunPt, old thread
	LDR		R1, [R0]		   ; 	R1 = RunPt
	STR		SP, [R1]		   ; 5) Save SP into TCB

;;;;
; "This ASM code can be replaced with a call to a subroutine 
;  to change the RunPt for the next thread"
;	LDR		R1, [R1, #4]	   ; 6) R1 = RunPt->next
;	STR		R1, [R0]		   ;	RunPt = R1
;;;;	

	PUSH    {R0,LR}           ; Save LR
    BL      Scheduler          ; Run next thread
	POP     {R0,LR}               ; Reload LR
    LDR     R0, =RunPt         ; Load next thread pointer value to R0  
	LDR     R1, [R0]		   ; 6) R1 = RunPt, new thread  
    LDR     SP, [R1]    	   ; 7) new thread SP, SP=RunPt->sp
	POP     {R4-R11}   		   ; 8) restore regs r4-11
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restpre R0-R3, R12, LR, PC, PSR
	

StartOS
; Copied from RTOS example. -Jonathan Arc
	LDR     R0, =RunPt         ; currently running thread
    LDR     R2, [R0]           ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer; 
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    ADD     SP,SP,#4           ; discard LR from initial stack
    POP     {LR}               ; start location
    ADD     SP,SP,#4           ; discard PSR
	
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
