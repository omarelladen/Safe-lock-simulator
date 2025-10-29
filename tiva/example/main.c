
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



