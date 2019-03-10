  .section .text
  .balign 2
  .thumb
  .syntax unified

  .global DisableInterrupts
  .global EnableInterrupts
  .global CheckInterrupts
  .global StartCritical
  .global EndCritical
  .global WaitForInterrupt

  .thumb_func
StartCritical:
  MRS   r0, PRIMASK

  .thumb_func
DisableInterrupts:
  CPSID I
  BX    lr

  .thumb_func
CheckInterrupts:
  MRS   r0, PRIMASK
  BX    lr

  .thumb_func
EnableInterrupts:
  CPSIE I
  BX    lr

  .thumb_func
EndCritical:
  MSR   PRIMASK, r0
  BX    lr

  .thumb_func
WaitForInterrupt:
  WFI
  BX    lr

  .global PendSV_Handler
  .global StartOS
  .global AtomicCompareAdd
  .global AtomicCompareAddNonBlocking
  .global AtomicAdd

# void PendSV_Handler(void) {
#   push(r4-r11);
#   RunPt->sp = sp;
#   RunPt = RunPt->next;
#   sp = RunPt->sp;
#   pop(r4-r11);
#   return;
# }

  .thumb_func
PendSV_Handler:
  CPSID I
  PUSH  {r4-r11}
  LDR   r0, =RunPt
  LDR   r1, [r0]
  STR   SP, [r1]
  LDR   r1, [r1, #4]
  STR   r1, [r0]
  LDR   sp, [r1]
  POP   {r4 - r11}
  CPSIE I
  BX    LR

  .thumb_func
StartOS:
  LDR   r0, =RunPt
  LDR   r1, [r0]
  LDR   sp, [r1]
  POP   {r4 - r11}
  POP   {r0 - r3}
  POP   {r12}
  POP   {lr}
  POP   {lr}
  POP   {r12}
  CPSIE I
  BX    LR

# void AtomicCompareAdd(long *semaPt, long value)
# Spinlocks until (*semaPt + value >= 0).

  .thumb_func
AtomicCompareAdd:
  LDREX r2, [r0]
  ADDS  r2, r1
  BLT   AtomicCompareAdd
  STREX r1, r2, [r0]
  CMP   r1, #0
  BNE   AtomicCompareAdd
  BX    lr

# long AtomicCompareAddNonBlocking(long *semaPt, long value)
# Returns (*semaPt + value); negative values indicate failure
# to acquire the lock.

  .thumb_func
AtomicCompareAddNonBlocking:
  LDREX   r2, [r0]
  ADDS    r2, r1
  ITT     LT
  CLREXLT
  BLT     Failure
  STREX   r1, r2, [r0]
  CMP     r1, #0
  BNE     AtomicCompareAddNonBlocking
Failure:
  MOV     r0, r2
  BX      lr

# long AtomicAdd(long *semaPt, long value)
# Returns (*semaPt + value); always stores the result to semaPt.

  .thumb_func
AtomicAdd:
  LDREX   r2, [r0]
  ADD     r2, r1
  STREX   r1, r2, [r0]
  CMP     r1, #0
  BNE     AtomicAdd
  MOV     r0, r2
  BX      lr

  .balign 2
  .end
