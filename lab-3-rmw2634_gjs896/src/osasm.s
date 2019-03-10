    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

    EXPORT PendSV_Handler
    EXPORT StartOS
    EXPORT AtomicCompareAdd
    EXPORT AtomicCompareAddNonBlocking
    EXPORT AtomicAdd
    EXTERN RunPt
    EXTERN NextPt


;Round-Robin Switching
;PendSV_Handler
;    CPSID I
;    PUSH {R4 - R11}
;    LDR R0, =RunPt
;    LDR R1, [R0]
;    STR SP, [R1]
;    LDR R1, [R1, #4]
;    STR R1, [R0]
;    LDR SP, [R1]
;    POP {R4 - R11}
;    CPSIE I
;    BX LR

;switch using chosen NextPt

PendSV_Handler
    CPSID I
    PUSH {R4 - R11}
    LDR R0, =RunPt
    LDR R1, [R0]
    STR SP, [R1]
    LDR R1, =NextPt
    LDR R2, [R1]
    STR R2, [R0]
    LDR SP, [R2]
    POP {R4 - R11}
    CPSIE I
    BX LR

StartOS
    LDR     R0, =RunPt         ; currently running thread
    LDR     R1, [R0]           ; R1 = value of RunPt
    LDR     SP, [R1]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R12}              ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

; long AtomicCompareAdd(long *semaPt, long value)
; Spinlocks until (*semaPt + value >= 0).
AtomicCompareAdd            ; do {
    LDREX r2, [r0]          ;   do {
    ADDS  r2, r1            ;     r2 = *semaPt + value;
    BLT   AtomicCompareAdd  ;   } while (r2 < 0);
    STREX r1, r2, [r0]      ;   r2 = __STREXW(r2, semaPt);
    CMP   r1, #0            ; } while (r2 != 0);
    BNE   AtomicCompareAdd  ;
    BX    lr                ; return;

; long AtomicCompareAddNonBlocking(long *semaPt, long value)
; Returns (*semaPt + value); negative values indicate failure
; to acquire the lock.
AtomicCompareAddNonBlocking
    LDREX   r2, [r0]
    ADDS    r1, r1
    ITT     LT
    CLREXLT
    BLT     Failure
    STREX   r1, r2, [r0]
    CMP     r1, #0
    BNE     AtomicCompareAddNonBlocking
Failure
    MOV     r0, r2
    BX      lr

; long AtomicAdd(long *semaPt, long value)
; Returns (*semaPt + value); always stores the result to semaPt.

AtomicAdd
    LDREX   r2, [r0]
    ADD     r2, r1
    STREX   r1, r2, [r0]
    CMP     r1, #0
    BNE     AtomicAdd
    MOV     r0, r2
    BX      lr

    ALIGN
    END
