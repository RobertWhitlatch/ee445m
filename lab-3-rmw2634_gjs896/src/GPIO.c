// GPIO.c

#include "CustomDefinitions.h"
#include "OS.h"
#include "Timers.h"

void PortE_Init(void){ 
    SYSCTL_RCGCGPIO_R |= 0x10;
    while((SYSCTL_RCGCGPIO_R&0x10)==0);
    GPIO_PORTE_DIR_R |= 0x0F;
    GPIO_PORTE_AFSEL_R &= ~0x0F;
    GPIO_PORTE_DEN_R |= 0x0F;
    GPIO_PORTE_PCTL_R = ~0x0000FFFF;
    GPIO_PORTE_AMSEL_R &= ~0x0F;
}


int priority1;
int priority2;

void SW_Init(void){
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_RCGCGPIO_R&0x20)==0);
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R = 0xFF;
    GPIO_PORTF_DIR_R &= 0x0E;
    GPIO_PORTF_AFSEL_R &= 0x0F;
    GPIO_PORTF_PUR_R |= 0x11;
    GPIO_PORTF_DEN_R |= 0x11;
    GPIO_PORTF_PCTL_R &= 0x0000FFF0;
    GPIO_PORTF_AMSEL_R &= 0x0E;
    GPIO_PORTF_IS_R &= 0x0E;                                        // Interrupt Sense -> Edge-Sensitive
    GPIO_PORTF_IBE_R &= 0x0E;                                       // Interrupt Both Edges -> No
    GPIO_PORTF_IEV_R |= 0x11;                                       // Interrupt Event -> Rising edge
    GPIO_PORTF_ICR_R = 0x11;                                        // Clear existing Interrupts
    //GPIO_PORTF_IM_R |= 0x11;                                        // Interrupt Mask -> Enable PF4
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (0x5 < 21);          // System-Level Priority -> 5
    NVIC_EN0_R |= 0x40000000;                                       // Enable Port F Interrupts at System-Level
}

void SW1_Arm(int priority){
    priority1 = (priority > 5) ? 5 : priority;
    GPIO_PORTF_ICR_R = 0x10;
    GPIO_PORTF_IM_R |= 0x10;
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (priority1 < 21);
    NVIC_EN0_R |= 0x40000000;
}

void SW2_Arm(int priority){
    priority2 = (priority > 5) ? 5 : priority;
    GPIO_PORTF_ICR_R = 0x01;
    GPIO_PORTF_IM_R |= 0x01;
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (priority2 < 21);
    NVIC_EN0_R |= 0x40000000;
}

void SW1_Debounce_Init(void){
    SYSCTL_RCGCWTIMER_R |= 0x10;                             // 0) activate TIMER4
    WTIMER4_CTL_R = 0x00000000;                              // 1) disable TIMER4A during setup
    WTIMER4_CFG_R = 0x00000000;                              // 2) configure for 32-bit mode
    WTIMER4_TAMR_R = 0x00000001;                             // 3) configure for periodic mode, default down-count settings
    WTIMER4_TAILR_R = 3999999*4;                             // 4) reload value
    WTIMER4_TAPR_R = 0;                                      // 5) bus clock resolution
    WTIMER4_ICR_R = 0x00000001;                              // 6) clear TIMER4A timeout flag
    WTIMER4_IMR_R = 0x00000001;                              // 7) arm timeout interrupt
    NVIC_PRI25_R = (NVIC_PRI25_R&0xFF00FFFF)|(priority1<<21);// 8) priority
    NVIC_EN3_R |= 0x00000040;                                // 9) enable IRQ 102/100 in NVIC
    WTIMER4_CTL_R = 0x00000001;                              // 10) enable TIMER4A
}

void SW2_Debounce_Init(void){
    SYSCTL_RCGCWTIMER_R |= 0x08;                             // 0) activate TIMER4
    WTIMER3_CTL_R = 0x00000000;                              // 1) disable TIMER4A during setup
    WTIMER3_CFG_R = 0x00000000;                              // 2) configure for 32-bit mode
    WTIMER3_TAMR_R = 0x00000001;                             // 3) configure for periodic mode, default down-count settings
    WTIMER3_TAILR_R = 3999999*4;                             // 4) reload value
    WTIMER3_TAPR_R = 0;                                      // 5) bus clock resolution
    WTIMER3_ICR_R = 0x00000001;                              // 6) clear TIMER4A timeout flag
    WTIMER3_IMR_R = 0x00000001;                              // 7) arm timeout interrupt
    NVIC_PRI25_R = (NVIC_PRI25_R&0xFFFFFF00)|(priority2<<21);// 8) priority
    NVIC_EN3_R |= 0x00000010;                                // 9) enable IRQ 102/100 in NVIC
    WTIMER3_CTL_R = 0x00000001;                              // 10) enable TIMER4A
}

void GPIOPortF_Handler(void){
    if(GPIO_PORTF_RIS_R & 0x10){
        GPIO_PORTF_IM_R &= 0x0F;
        SW1_Debounce_Init();
    }else if(GPIO_PORTF_RIS_R & 0x01){
        GPIO_PORTF_IM_R &= 0x1E;
        SW2_Debounce_Init();
    }
}

void (*job1)(void);
void WideTimer4A_Handler(void){
    WTIMER4_IMR_R = 0x00000000;
    job1();
    SW1_Arm(priority1);
}

void (*job2)(void);
void WideTimer3A_Handler(void){
    WTIMER3_IMR_R = 0x00000000;
    job2();
    SW2_Arm(priority2);
}
