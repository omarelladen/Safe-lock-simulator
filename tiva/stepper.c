#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTK  (0x200)  //bit 9
#define GPIO_PORTM  (0x800)  //bit 11

void SysTick_Wait1ms(uint32_t delay);


void Stepper_Init(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7; // Liga clock do Port H
    delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTH_AHB_DIR_R |= 0x0F;  // PH0–PH3 como saída
    GPIO_PORTH_AHB_DEN_R |= 0x0F;  // Digital enable
}

void Stepper_StepCW(void) {
    GPIO_PORTH_AHB_DATA_R = 0x0E; 
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x0D; 
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x0B; 
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x07; 
    SysTick_Wait1ms(5);
}

void Stepper_StepCCW_HalfStep(void) {
    GPIO_PORTH_AHB_DATA_R = 0x01;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x03;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x02;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x06;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x04;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x0C;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x08;
    SysTick_Wait1ms(5);
    GPIO_PORTH_AHB_DATA_R = 0x09;
    SysTick_Wait1ms(5);
}


//fechar: 2 voltas sentido anti horario no modo meio passo
//abrir: 2 voltas sentido horario passo completo 

