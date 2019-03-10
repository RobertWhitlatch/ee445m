// PriorityQueue.h
// 0=highest priority, PQUEUE_MAX_PRI=lowest priority

#define PQUEUE_MAX_PRI (8*sizeof(unsigned int)-1)

typedef void PQItem_t;

typedef struct PQueue_s {
    unsigned int flags;
    PQItem_t *items[PQUEUE_MAX_PRI + 1];
} PQueue_t;

void PQ_Init(PQueue_t *this);
void PQ_Insert(PQueue_t *this, PQItem_t *item, unsigned int priority);
int PQ_InsertWithin(PQueue_t *this,
                    PQItem_t *item,
                    unsigned int minPriority,
                    unsigned int maxPriority);
PQItem_t *PQ_Peek(PQueue_t *this);
PQItem_t *PQ_Pop(PQueue_t *this);
