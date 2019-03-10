# Implementation Notes

## Hardware Utilization (*=Interrupting)


### I/O

- Ain{0-11}
- *GPIOPortF: SW1/SW2/LED
- *UART0: Console
- SSI0: ST7735 + SD Card

### Internal

- *ADC0Seq3: ADC\_Sample()
- ADC1Seq3: ADC\_In()
- Timer2: ADC\_Sample()
- *Timer4: Periodic Task #1
- *Timer5: Periodic Task #2
- *WideTimer3: SW2 Debounce
- *WideTimer4: SW1 Debounce
- WideTimer5: System Time
- *SysTick: (OS)
- *PendSV: (OS)
- *HardFault: (OS)
