#include <LiquidCrystal.h>
#include "defines.h"

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

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

void setup()
{
    pinMode(PIN_LCD_LIGHT, INPUT);
    pinMode(PIN_SHIELD_BTS, INPUT);

    pinMode(PIN_D3, INPUT_PULLUP);  // L1
	pinMode(PIN_D2, INPUT_PULLUP);  // L2
    pinMode(PIN_D1, INPUT_PULLUP);  // L3
    pinMode(PIN_D0, INPUT_PULLUP);  // L4
    pinMode(PIN_A1,   INPUT);  // C1
    pinMode(PIN_CLK,  INPUT);  // C2
    pinMode(PIN_MISO, INPUT);  // C3
    pinMode(PIN_MOSI, INPUT);  // C4


    lcd.begin(16, 2);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre aberto,di-"));
	lcd.setCursor(0, 1);
	lcd.print(F("gite nova senha"));
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
		else
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
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print(F("Cofre fechado"));
					
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
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print(F("Cofre travado"));
				}
			}
		}
	}
}

void update_pw()
{
	int8_t i;
	for (i=0; i < 4; i++)
		g_pw[i] = g_pw_try[i];
}

int8_t pw_is_correct()
{
	int8_t i;
	for (i=0; i < 4; i++)
		if (g_pw_try[i] != g_pw[i])
			return 0;
	return 1;
}

int8_t pw_master_is_correct()
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
			lcd.setCursor(11 + g_pw_try_index, 1);
			lcd.print(num);

			g_pw_try[g_pw_try_index] = num;
			g_pw_try_index++;
		}
		else
		{
			if (g_sw1_pressed)
			{
				lcd.setCursor(11 + g_pw_try_index, 1);
				lcd.print(num);

				g_pw_try[g_pw_try_index] = num;
				g_pw_try_index++;
			}
		}
	}
}

void open_safe()
{
	g_safe_is_open = 1;
	g_wrong_tries = 0;

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre abrindo"));

	delay(500);
	g_time += 500;

	// motor girar 2 voltas no sentido horário no modo passo completo

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre aberto,di-"));
	lcd.setCursor(0, 1);
	lcd.print(F("gite nova senha"));
}

void close_safe()
{
	g_safe_is_open = 0;

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre fechando"));

	delay(1000);
	g_time += 1000;

	// motor girar 2 voltas no sentido anti-horario no modo meio passo

	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre fechado"));
}

void lock_safe()
{
	g_safe_is_locked = 1;

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre travado"));
}

void blink_leds()
{
}

void check_kb_press()
{
	int i;

    for (i = 1; i <= 4; i++)
    {
    	// Z
	    pinMode(PIN_A1,   INPUT);  // C1
	    pinMode(PIN_CLK,  INPUT);  // C2
	    pinMode(PIN_MISO, INPUT);  // C3
	    pinMode(PIN_MOSI, INPUT);  // C4
		delay(1);


	    if (i == 1)
		{
			pinMode(PIN_A1, OUTPUT);
			delay(1);
			digitalWrite(PIN_A1, LOW);
			delay(1);
		}
		else if (i == 2)
		{
			pinMode(PIN_CLK, OUTPUT);
			delay(1);
			digitalWrite(PIN_CLK, LOW);
			delay(1);
		}
		else if (i == 3)
		{
			pinMode(PIN_MISO, OUTPUT);
			delay(1);
			digitalWrite(PIN_MISO, LOW);
			delay(1);
		}
		else if (i == 4)
		{
			pinMode(PIN_MOSI, OUTPUT);
			delay(1);
			digitalWrite(PIN_MOSI, LOW);
			delay(1);
		}

	    
	    if (digitalRead(PIN_D3) == LOW)  // L1
	    {
	      handle_kb_press(1, i);
	      // while (digitalRead(PIN_D3) == LOW);
	    }
	    else if (digitalRead(PIN_D2) == LOW)  // L2
	    {
	      handle_kb_press(2, i);
	      // while (digitalRead(PIN_D2) == LOW);
	    }
	    else if (digitalRead(PIN_D1) == LOW)  // L3
	    {
	      handle_kb_press(3, i);
	      // while (digitalRead(PIN_D1) == LOW);
	    }
	    else if (digitalRead(PIN_D0) == LOW)  // L4
	    {
	      handle_kb_press(4, i);
	      // while (digitalRead(PIN_D0) == LOW);
	    }
	    else
	    	handle_kb_press(-1, i);
	}
	delay(10);
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
	if ((g_time - g_kb_delay) > DEBOUNCE_TIME*4)  // millis()
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

void bt_pressed(int8_t bt)
{	
	if (g_safe_is_locked)
	{
	    if (bt == BT_SELECT)
			g_sw1_pressed = 1;
	}
}

int8_t check_bt_press()
{
    int8_t bt;
    const int16_t bt_analog_value = analogRead(PIN_SHIELD_BTS);

    if (bt_analog_value < RIGHT_THRESHOLD)
        bt = BT_RIGHT;
    else if (bt_analog_value < DOWN_THRESHOLD)
        bt = BT_UP;
    else if (bt_analog_value < UP_THRESHOLD)
        bt = BT_DOWN;
    else if (bt_analog_value < LEFT_THRESHOLD)
        bt = BT_LEFT;
    else if (bt_analog_value < SEL_THRESHOLD)
        bt = BT_SELECT;
    else
        bt = BT_NONE;

    return bt;
}

void handle_bt_press(const int8_t bt)
{
	bt_pressed(bt);
}


void loop()
{
    const int8_t bt_pressed = check_bt_press();
    handle_bt_press(bt_pressed);

    check_kb_press();

    if (g_safe_is_locked)
    	blink_leds();
}
