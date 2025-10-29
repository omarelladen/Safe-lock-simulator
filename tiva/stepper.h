#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

// Inicializa o motor (configura os pinos PH0–PH3)
void Stepper_Init(void);

// Move o motor 1 passo no sentido horário
void Stepper_StepCW(void);

// Move o motor 1 passo no sentido anti-horário
void Stepper_StepCCW(void);

// Gira o motor 'n' passos em um sentido específico
void Stepper_Rotate(uint32_t steps, uint8_t direction);

// Define um delay entre os passos (em ms)
void Stepper_Delay(uint32_t ms);


#endif
