// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>
#include "stepper.h"
#include "lcd.h"

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Stepper_Init();
	LCD_Init();
  LCD_SetCursor(0,0);
  LCD_WriteString("Ola");
  LCD_SetCursor(1,0);
  LCD_WriteString(":)");
	while (1)
	{
		Stepper_StepCW();
	}
}


