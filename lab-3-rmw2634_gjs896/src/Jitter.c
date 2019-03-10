// Jitter.c

#include <string.h> // memset()

#include "CustomDefinitions.h" // MIN(), MAX()
#include "OS.h" // OS_Time(), OS_TimeDifference()

#include "Jitter.h"

Jitter_t periodic_jitter[2];

void JitterReset(Jitter_t *this) {
    memset(this, 0, sizeof(*this));
}

void JitterUpdate(Jitter_t *this) {
    if(this->lastTime == 0){
        this->lastTime = OS_Time();
        return;
    }
    unsigned long now = OS_Time();
    unsigned long delta = OS_TimeDifference(this->lastTime, now);
    unsigned long jitter = (delta > this->period) ?
                           delta - this->period :
                           this->period - delta;

    this->lastTime = now;
    this->maxValue = MAX(jitter, this->maxValue);
    this->histogram[MIN(jitter, JITTER_NUM_BINS-1)]++;
}

void Jitter(){
    fprintf(uart, "Periodic Thread 1 Max Jitter = %lu\nPeriodic Thread 2 Max Jitter = %lu\n", periodic_jitter[0].maxValue, periodic_jitter[1].maxValue);
}
