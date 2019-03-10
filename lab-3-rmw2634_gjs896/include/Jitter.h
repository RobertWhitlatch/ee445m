// Jitter.h

#define JITTER_NUM_BINS 64

typedef struct Jitter_s {
    unsigned long lastTime;
    unsigned long period;
    unsigned long maxValue;
    unsigned long histogram[JITTER_NUM_BINS];
} Jitter_t;

extern Jitter_t periodic_jitter[2];

void JitterReset(Jitter_t *this);
void JitterUpdate(Jitter_t *this);
