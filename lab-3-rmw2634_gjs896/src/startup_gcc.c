//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
//
//*****************************************************************************

//*****************************************************************************
// Last modified February 7, 2019 by Clara Short <clarity@utexas.edu>
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************

static void DefaultISR(void) __attribute__((noreturn));
extern void Reset_Handler(void) __attribute__((noreturn));
extern void NMI_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void HardFault_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void MPUFault_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void BusFault_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UsageFault_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SVCall_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Debug_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PendSV_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SysTick_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortA_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortB_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortC_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortD_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortE_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SSI0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void I2C0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM0Fault_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM00_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM01_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM02_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void QEI0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC0Seq0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC0Seq1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC0Seq2_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC0Seq3_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Watchdog_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer0A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer0B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer1A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer1B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer2A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer2B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Comp0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Comp1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SysCtl_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Flash_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void GPIOPortF_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART2_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SSI1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer3A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer3B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void I2C1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void QEI1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void CAN0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void CAN1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Hibernate_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void USB0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM03_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UDMA_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UDMAError_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC1Seq0_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC1Seq1_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC1Seq2_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void ADC1Seq3_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SSI2_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SSI3_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART3_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART4_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART5_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART6_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void UART7_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void I2C2_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void I2C3_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer4A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer4B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer5A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void Timer5B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer0A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer0B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer1A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer1B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer2A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer2B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer3A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer3B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer4A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer4B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer5A_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void WideTimer5B_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void SysExc_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM10_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM11_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM12_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM13_Handler(void) __attribute__((weak, alias("DefaultISR")));
extern void PWM1Fault_Handler(void) __attribute__((weak, alias("DefaultISR")));

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void) __attribute__((noreturn));

//*****************************************************************************
//
// Reserve space for the system stack and define the initial stack pointer.
//
//*****************************************************************************
static uint32_t pui32Stack[64];
#define STACK_TOP ((uint32_t)pui32Stack + sizeof(pui32Stack))

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
    (void (*)(void))STACK_TOP,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MPUFault_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVCall_Handler,
    Debug_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    GPIOPortA_Handler,
    GPIOPortB_Handler,
    GPIOPortC_Handler,
    GPIOPortD_Handler,
    GPIOPortE_Handler,
    UART0_Handler,
    UART1_Handler,
    SSI0_Handler,
    I2C0_Handler,
    PWM0Fault_Handler,
    PWM00_Handler,
    PWM01_Handler,
    PWM02_Handler,
    QEI0_Handler,
    ADC0Seq0_Handler,
    ADC0Seq1_Handler,
    ADC0Seq2_Handler,
    ADC0Seq3_Handler,
    Watchdog_Handler,
    Timer0A_Handler,
    Timer0B_Handler,
    Timer1A_Handler,
    Timer1B_Handler,
    Timer2A_Handler,
    Timer2B_Handler,
    Comp0_Handler,
    Comp1_Handler,
    0,
    SysCtl_Handler,
    Flash_Handler,
    GPIOPortF_Handler,
    0,
    0,
    UART2_Handler,
    SSI1_Handler,
    Timer3A_Handler,
    Timer3B_Handler,
    I2C1_Handler,
    QEI1_Handler,
    CAN0_Handler,
    CAN1_Handler,
    0,
    0,
    Hibernate_Handler,
    USB0_Handler,
    PWM03_Handler,
    UDMA_Handler,
    UDMAError_Handler,
    ADC1Seq0_Handler,
    ADC1Seq1_Handler,
    ADC1Seq2_Handler,
    ADC1Seq3_Handler,
    0,
    0,
    0,
    0,
    0,
    SSI2_Handler,
    SSI3_Handler,
    UART3_Handler,
    UART4_Handler,
    UART5_Handler,
    UART6_Handler,
    UART7_Handler,
    0,
    0,
    0,
    0,
    I2C2_Handler,
    I2C3_Handler,
    Timer4A_Handler,
    Timer4B_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Timer5A_Handler,
    Timer5B_Handler,
    WideTimer0A_Handler,
    WideTimer0B_Handler,
    WideTimer1A_Handler,
    WideTimer1B_Handler,
    WideTimer2A_Handler,
    WideTimer2B_Handler,
    WideTimer3A_Handler,
    WideTimer3B_Handler,
    WideTimer4A_Handler,
    WideTimer4B_Handler,
    WideTimer5A_Handler,
    WideTimer5B_Handler,
    SysExc_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    PWM10_Handler,
    PWM11_Handler,
    PWM12_Handler,
    PWM13_Handler,
    PWM1Fault_Handler};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void Reset_Handler(void) {
  uint32_t *pui32Src, *pui32Dest;

  //
  // Copy the data segment initializers from flash to SRAM.
  //
  pui32Src = &_ldata;
  for (pui32Dest = &_data; pui32Dest < &_edata;) {
    *pui32Dest++ = *pui32Src++;
  }

  //
  // Zero fill the bss segment.
  // Don't replace this with C code or it will generate a BusFault.
  //
  __asm(
    " ldr r0, =_bss\n"
    " ldr r1, =_ebss\n"
    " mov r2, #0\n"
    " .thumb_func\n"
    "zero_loop:\n"
    " cmp r0, r1\n"
    " it lt\n"
    " strlt r2, [r0], #4\n"
    " blt zero_loop");

  /*
    //
    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).  Any configuration of the floating-point unit using DriverLib
    // APIs must be done here prior to the floating-point unit being enabled.
    //
    // Note that this does not use DriverLib since it might not be included in
    // this project.
    //
    HWREG(NVIC_CPAC) =
        ((HWREG(NVIC_CPAC) & ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
         NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);
  */
  //
  // Call the application's entry point.
  //
  main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void DefaultISR(void) { while(1); }
