    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

    EXPORT PendSV_Handler 
;   EXPORT SysTick_Handler
    EXPORT StartOS
    EXTERN RunPt
    EXTERN NextPt


;Round-Robin Switching
;SysTick_Handler
PendSV_Handler 
    CPSID I
    PUSH {R4 - R11}
    LDR R0, =RunPt
    LDR R1, [R0]
    STR SP, [R1]
    LDR R1, [R1, #4]
    STR R1, [R0]
    LDR SP, [R1]
    POP {R4 - R11}
    CPSIE I
    BX LR

;switch using chosen NextPt

;PendSV_Handler
;    CPSID I
;    PUSH {R4 - R11}
;    LDR R0, =RunPt
;    LDR R1, [R0]
;    STR SP, [R1]
;    LDR R1, =NextPt
;    LDR R2, [R1]
;    STR R2, [R0]
;    LDR SP, [R2]
;    POP {R4 - R11}
;    CPSIE I
;    BX LR

StartOS
    LDR     R0, =RunPt         ; currently running thread
    LDR     R1, [R0]           ; R1 = value of RunPt
    LDR     SP, [R1]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R12}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END