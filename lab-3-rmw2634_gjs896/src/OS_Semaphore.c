#include <stdbool.h>
// #include <stdlib.h> // malloc(), free()

#include "CustomDefinitions.h"
#include "OS.h"

extern TCB_t *RunPt;

// ******** linked list functions ********

// Insert a thread at the end of the list.
void _append(ThreadList_t **list, TCB_t *tcb) {
  while (*list) { list = &((*list)->cdr); }
  // *list = malloc(sizeof(ThreadList_t));
  (*list)->car = tcb;
  (*list)->cdr = 0;
}

// Insert a thread behind all other threads of the same or higher priority.
void _insert(ThreadList_t **list, TCB_t *tcb) {
  while (*list && ((*list)->car->priority <= tcb->priority)) {
    list = &((*list)->cdr);
  }
  if (!*list) {
    _append(list, tcb);
    return;
  }
  ThreadList_t *newItem = 0; // = malloc(sizeof(ThreadList_t));
  newItem->car = (*list)->car;
  newItem->cdr = (*list)->cdr;
  (*list)->car = tcb;
  (*list)->cdr = newItem;
}

// Remove and return the first thread from the list.
TCB_t *_pop(ThreadList_t **list) {
  if (!*list) { return 0; }
  TCB_t *car = (*list)->car;
  ThreadList_t *cdr = (*list)->cdr;
  // free(*list);
  *list = cdr;
  return car;
}

// ******** OS_InitSemaphore ************
// initialize semaphore
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4Type* semaPt, long value) {
  semaPt->Value = value;
  semaPt->blockedThreads = 0;
}

// ******** OS_Wait ************
// decrement semaphore
// Lab2 spinlock
// Lab3 block if less than zero
// input:  pointer to a counting semaphore
// output: none
// void OS_Wait(Sema4Type* semaPt) { AtomicCompareAdd(&(semaPt->Value), -1); }
void OS_Wait(Sema4Type *semaPt) {
  bool prevIFlag = StartCritical();
  if (AtomicAdd(&(semaPt->Value),-1) < 0) {
    RunPt->blocked = semaPt;
    _insert(&(semaPt->blockedThreads), RunPt);
    OS_Suspend(); // causes a PendSV interrupt once interrupts are re-enabled
  }
  EndCritical(prevIFlag);
}

// ******** OS_Signal ************
// increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(Sema4Type* semaPt) {
  bool prevIFlag = StartCritical();
  if (AtomicAdd(&(semaPt->Value),1) <= 0) {
    TCB_t *unblockedThread = _pop(&(semaPt->blockedThreads));
    unblockedThread->blocked = 0;
    if (unblockedThread->priority < RunPt->priority) {
      // TODO: ensure we don't suspend a background (periodic) thread
      OS_Suspend();
    }
  }
  EndCritical(prevIFlag);
}

// ******** OS_bWait ************
// Lab2 spinlock, set to 0
// Lab3 block if less than zero
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4Type* semaPt) { OS_Wait(semaPt); }

// ******** OS_bSignal ************
// Lab2 spinlock, set to 1
// Lab3 wakeup blocked thread if appropriate
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4Type* semaPt) { OS_Signal(semaPt); }

// ******** OS_Acquire ********
// decrement semaphore, return false if less than zero
// input:  pointer to a counting semaphore
// output: true if we acquired the semaphore, false if we failed
bool OS_Acquire(Sema4Type* semaPt) {
  return (AtomicCompareAddNonBlocking(&(semaPt->Value), -1) >= 0);
}

// ******** OS_SpinWait ********
// spinlock (use for kernel mutexes only!)
void OS_SpinWait(Sema4Type* semaPt) {
  while (!OS_Acquire(semaPt)) {}
}
