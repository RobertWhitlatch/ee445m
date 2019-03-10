// main.c
// Selects which test to run.

#include "Lab3.h"
#include "OS.h"
#include "Output.h"
#include "PLL.h"

int final[16];

void HardFault_Handler(void){
    final[0] = OS_Time();
    while(1);
}

int main(void) {
#if TESTMAIN == 5
    Testmain5();
#elif TESTMAIN == 6
    Testmain6();
#elif TESTMAIN == 7
    Testmain7();
#else
    realmain();
#endif // TESTMAIN
}
