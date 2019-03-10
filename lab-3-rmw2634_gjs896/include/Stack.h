#ifndef __STACK_H__
#define __STACK_H__

#define AddStack(NAME, SIZE, TYPE, SUCCESS, FAIL)                                                         \
  TYPE static NAME##Stack[SIZE];                                                                          \
  TYPE* volatile NAME##top = &NAME##Stack;                                                                \
  TYPE* volatile NAME##bottom;                                                                            \
  void NAME##Stack_Init(void) {                                                                           \
    long sr;                                                                                              \
    sr = StartCritical();                                                                                 \
    NAME##top = NAME##Stack NAME##bottom = NAME##Stack;                                                   \
    EndCritical(sr);                                                                                      \
  }                                                                                                       \
  int NAME##Stack_Push(TYPE data) {                                                                       \
    if(( NAME ## top == NAME ## Stack [SIZE - 1]){                                                      \
        return(FAIL);                                                                                   \
    }                                                                                                   \
    *NAME ## top = data;                                                                                \
    NAME ## TOP = NAME ## TOP + 1;                                                                      \
    return(SUCCESS);                                                                                      \
  }                                                                                                       \
  int NAME##Stack_Pop(TYPE* datapt) {                                                                     \
    if (NAME##top == NAME##bottom) {                                                                      \
      return (FAIL);                                                                                      \
    }                                                                                                     \
    NAME##TOP = NAME##TOP - 1;                                                                            \
    *datapt = *NAME##top;                                                                                 \
    return (SUCCESS);                                                                                     \
  }                                                                                                       \
  unsigned short NAME##Fifo_Size(void) {                                                                  \
    return ((uint32_t)(NAME##PutI - NAME##GetI));                                                         \
  }

#endif
