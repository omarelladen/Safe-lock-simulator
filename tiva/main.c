// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>
#include "stepper.h"
#include "lcd.h"
#include "tm4c1294ncpdt.h"

#define BT_NONE 0
#define DEBOUNCE_TIME 200

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
uint32_t PortL_Input(void);
void PortM_Output(uint32_t valor);
void PortN_Output(uint32_t leds);
void Pisca_leds(void);
void update_pw(void);
void open_safe(void);
void close_safe(void);
int8_t pw_is_correct(void);
int8_t pw_master_is_correct(void);
void lock_safe(void);
void handle_kb_press(const int8_t line, const int8_t col);
void check_kb_press(void);
void sw1_pressed(void);

int8_t g_light_is_on = 1;
int8_t g_prev_kb_state = BT_NONE;
unsigned long g_kb_delay = 0;
int8_t g_wrong_tries = 0;
int8_t g_safe_is_open = 1;
int8_t g_pw[4];
const int8_t g_pw_master[4] = {1,2,3,4};
int8_t g_pw_try[4];
int8_t g_pw_try_index = 0;
int8_t g_safe_is_locked = 0;
int8_t g_sw1_pressed = 0;

int64_t g_time = 0;

int main(void)
{

	
	
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Stepper_Init();
	LCD_Init();
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre aberto,di-");
	LCD_SetCursor(1, 0);
	LCD_WriteString("gite nova senha");
	while (1)
	{
    check_kb_press();
		
    //if (g_safe_is_locked)
    	//blink_leds();
	}
}

void Pisca_leds(void)
{
	PortN_Output(0x2);
	SysTick_Wait1ms(250);
	PortN_Output(0x1);
	SysTick_Wait1ms(250);
}

void kb_hashtag_pressed()
{
	if (g_pw_try_index == 4)
	{
		g_pw_try_index = 0;
		if (g_safe_is_open)
		{
			update_pw();
			close_safe();
		}
		else  // ou n precisa do # pra abrir?
		{
			if (!g_safe_is_locked)
			{
				if (pw_is_correct())
				{
					g_wrong_tries = 0;
					open_safe();
				}
				else
				{
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_WriteString("Cofre fechado");
					
					g_wrong_tries++;
					if (g_wrong_tries == 3)
						lock_safe();
				}				
			}
			else
			{
				if (pw_master_is_correct())
				{
					g_sw1_pressed = 0;
					g_safe_is_locked = 0;
					g_wrong_tries = 0;
					open_safe();
				}
				else
				{
					g_sw1_pressed = 0;
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_WriteString("Cofre travado");
				}
			}
		}
	}
}

void update_pw(void)
{
	int8_t i;
	for (i=0; i < 4; i++)
		g_pw[i] = g_pw_try[i];
}

int8_t pw_is_correct(void)  // parece q a senha mestre eh so pra travamento, n sei se precisa colocar o teste dela aqui
{
	int8_t i;
	for (i=0; i < 4; i++)
		if (g_pw_try[i] != g_pw[i])
			return 0;
	return 1;
}

int8_t pw_master_is_correct(void)
{
	int i;
	for (i=0; i < 4; i++)
		if (g_pw_try[i] != g_pw_master[i])
			return 0;
	return 1;
}

void kb_num_pressed(int8_t num)
{
	if (g_pw_try_index < 4)
	{
		if (!g_safe_is_locked)
		{
			//LCD_SetCursor(11+g_pw_try_index, 1);
			//lcd.print(num);

			g_pw_try[g_pw_try_index] = num;
			g_pw_try_index++;
		}
		else
		{
			if (g_sw1_pressed)
			{
				//LCD_SetCursor(11+g_pw_try_index, 1);
				//lcd.print(num);

				g_pw_try[g_pw_try_index] = num;
				g_pw_try_index++;
			}
		}
	}
}

void open_safe(void)
{
	
	int32_t timer = 100000;
	int32_t timer_temp = 0;
	
	g_safe_is_open = 1;
	g_wrong_tries = 0;

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre abrindo");

	SysTick_Wait1ms(500);
	g_time += 500;
	
	Stepper_StepCW(2000);
	
	// motor girar 2 voltas no sentido horário no modo passo completo

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre aberto,di-");
	LCD_SetCursor(1, 0);
	LCD_WriteString("gite nova senha");
}

void close_safe(void)
{
	int32_t timer = 100000;
	int32_t timer_temp = 0;
	g_safe_is_open = 0;

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre fechando");

	SysTick_Wait1ms(1000);
	g_time += 1000;
	
	Stepper_StepCCW_HalfStep(1000);
	// motor girar 2 voltas no sentido anti-horario no modo meio passo

	LCD_Clear();	
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre fechado");
}

void lock_safe(void)
{
	g_safe_is_locked = 1;

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_WriteString("Cofre travado");
}

void blink_leds()
{
}



void check_kb_press(void)
{
	int i;

    for (i = 1; i <= 4; i++)
    {
    	// Z
			GPIO_PORTM_DIR_R = 0x07;
			SysTick_Wait1ms(1);


	    if (i == 1)
		{
			GPIO_PORTM_DIR_R = 0x17;
			SysTick_Wait1ms(1);
			PortM_Output(0x00);
			SysTick_Wait1ms(1);
		}
		else if (i == 2)
		{
			GPIO_PORTM_DIR_R = 0x27;
			SysTick_Wait1ms(1);
			PortM_Output(0x00);
			SysTick_Wait1ms(1);
		}
		else if (i == 3)
		{
			GPIO_PORTM_DIR_R = 0x47;
			SysTick_Wait1ms(1);
			PortM_Output(0x00);
			SysTick_Wait1ms(1);
		}
		else if (i == 4)
		{
			GPIO_PORTM_DIR_R = 0x87;
			SysTick_Wait1ms(1);
			PortM_Output(0x00);
			SysTick_Wait1ms(1);
		}

	    
	    if ((PortL_Input() | ~0x01) == ~0x01)  // L1
	    {
	      handle_kb_press(1, i);
	      // while (digitalRead(PIN_D3) == LOW);
	    }
	    else if ((PortL_Input() | ~0x02) == ~0x02)  // L2
	    {
	      handle_kb_press(2, i);
	      // while (digitalRead(PIN_D2) == LOW);
	    }
	    else if ((PortL_Input() | ~0x04) == ~0x04)  // L3
	    {
	      handle_kb_press(3, i);
	      // while (digitalRead(PIN_D1) == LOW);
	    }
	    else if ((PortL_Input() | ~0x08) == ~0x08)  // L4
	    {
	      handle_kb_press(4, i);
	      // while (digitalRead(PIN_D0) == LOW);
	    }
	    else
	    	handle_kb_press(-1, i);
	}
	SysTick_Wait1ms(10);
	g_time += 20;
}

void handle_kb_press(const int8_t line, const int8_t col)
{
	char dig;

	switch (line)
	{
	case (1):
		switch (col)
		{
		case (1):
			dig = 1;
			break;
		case (2):
			dig = 2;
			break;
		case (3):
			dig = 3;
			break;
		case (4):
			dig = -1;
			break;
		}
		break;
	case (2):
		switch (col)
		{
		case (1):
			dig = 4;
			break;
		case (2):
			dig = 5;
			break;
		case (3):
			dig = 6;
			break;
		case (4):
			dig = -1;
			break;
		}
		break;
	case (3):
		switch (col)
		{
		case (1):
			dig = 7;
			break;
		case (2):
			dig = 8;
			break;
		case (3):
			dig = 9;
			break;
		case (4):
			dig = -1;
			break;
		}
		break;
	case (4):
		switch (col)
		{
		case (1):
			dig = -1;
			break;
		case (2):
			dig = 0;
			break;
		case (3):
			dig = '#';
			break;
		case (4):
			dig = -1;
			break;
		}
		break;
	default:
		dig = BT_NONE;
		break;
	}

	// Debounce
	if ((g_time - g_kb_delay) > DEBOUNCE_TIME)  // millis()
	{
	    if ((dig == BT_NONE) && (g_prev_kb_state != BT_NONE))
	    {
	        if (g_prev_kb_state == '#')
	        	kb_hashtag_pressed();
	        else if (g_prev_kb_state >= 0 && g_prev_kb_state <= 9)
	        	kb_num_pressed(g_prev_kb_state);

	        g_kb_delay = g_time;  // millis()
	    }
	}
	g_prev_kb_state = dig;
}

void sw1_pressed(void)
{	
	if (g_safe_is_locked)
	{
			g_sw1_pressed = 1;
	}
	

}

