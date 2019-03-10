// PriorityQueue.c
// 0=highest priority, PQUEUE_MAX_PRI=lowest priority

#include "PriorityQueue.h"

void PQ_Init(PQueue_t *this) { this->flags = 0; }

void PQ_Insert(PQueue_t *this, PQItem_t *item, unsigned int priority) {
    this->items[priority] = item;
    this->flags |= 1 << (PQUEUE_MAX_PRI - priority);
}

// try to insert an item somewhere between the specified priorities
// return 1 on success, 0 on failure
int PQ_InsertWithin(PQueue_t *this,
                    PQItem_t *item,
                    unsigned int minPriority,
                    unsigned int maxPriority) {
    unsigned int flagMask = (1 << (PQUEUE_MAX_PRI - minPriority + 1)) -
                            (1 << (PQUEUE_MAX_PRI - maxPriority));
    unsigned int maskedFlags = flagMask & ~(this->flags);
    if (!maskedFlags) { return 0; }
    PQ_Insert(this, item, PQUEUE_MAX_PRI - __builtin_clz(maskedFlags));
    return 1;
}

PQItem_t *PQ_Peek(PQueue_t *this) {
    if (!(this->flags)) { return 0; }
    unsigned int inv_priority = __builtin_clz(this->flags);
    return this->items[PQUEUE_MAX_PRI - inv_priority];
}

PQItem_t *PQ_Pop(PQueue_t *this) {
    if (!(this->flags)) { return 0; }
    unsigned int inv_priority = __builtin_clz(this->flags);
    this->flags &=~ (1 << inv_priority);
    return this->items[PQUEUE_MAX_PRI - inv_priority];
}
