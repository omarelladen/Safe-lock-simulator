#include <LiquidCrystal.h>
#include "defines.h"

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

int8_t g_light_is_on = 1;
int8_t g_prev_bt_state = BT_NONE;
unsigned long g_bt_delay = 0;

int8_t g_prev_kb_state = BT_NONE;
unsigned long g_kb_delay = 0;
int8_t g_wrong_tries = 0;
int8_t g_safe_is_open = 1;
int8_t g_pw[4];
const int8_t g_pw_master[4] = {1,2,3,4};
int8_t g_pw_try[4];
int8_t g_pw_try_index = 0;
int8_t g_safe_is_locked = 0;


// Cofre aberto, digite nova senha
// fechar: senha de 4 digitos + # -> esperar 1s, motor girar 2 voltas no sentido anti-horario no modo meio passo, Cofre fechando
// Cofre fechado
// senha previamente cadastrada digitada corretamente -> motor girar 2 voltas no sentido horário no modo passo completo, Cofre abrindo
// Cofre aberto, digite nova senha
// senha mestra inicializada como 1234 para abrir em caso de travamento
// cofre fechado e senha digitada incorretamente 3x -> cofre travará
// Os LEDs da PAT piscando e “Cofre Travado”
// aberto pressionando USR_SW1 acionada por interrupcao de GPIO.
// Em seguida a senha mestra devera ser requisitada. A senha mestra so podera ser digitada se a chave USR_SW1 for pressionada
// Se a senha mestra for digitada corretamente, os 8 LEDs da PAT devem parar de piscar, e, em seguida, o cofre deve ser aberto, indicado pelo LCD como do passo 5.
//​Utilizar o algoritmo de varredura para realizar a leitura das teclas do teclado matricial.
// Para colocar um pino em alta impedância atribuir o
// respectivo bit do GPIO_DIR para entrada.
// A cada troca de entrada para saída e saída para entrada, esperar no mínimo 1 ms.


void setup()
{
    pinMode(PIN_LCD_LIGHT, INPUT);

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
					g_wrong_tries++;
					if (g_wrong_tries == 3)
					{
						lock_safe();
					}
				}
				
			}
			else
			{
				if (pw_master_is_correct())
				{
					g_wrong_tries = 0;
					open_safe();
				}
			}
		}
	}

}

void sw1_pressed()
{
	// testar senha apenas qnd sw1 pressionada (???)
	// testar teclado e  preencher o g_pw_try
}

void update_pw()
{
	int8_t i;
	for (i=0; i < 4; i++)
		g_pw[i] = g_pw_try[i];
}

int8_t pw_is_correct()  // parece q a senha mestre eh so pra travamento, n sei se precisa colocar o teste dela aqui
{
	int8_t i;
	for (i=0; i < 4; i++)
		if (g_pw_try[i] != g_pw[i])
			return 0;
	return 1;
}

int8_t pw_master_is_correct()  // parece q a senha mestre eh so pra travamento, n sei se precisa colocar o teste dela aqui
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
		lcd.setCursor(15, 1);
		lcd.print(num);
		
		lcd.setCursor(14, 1);
		lcd.print(g_pw_try_index);
		
		
		g_pw_try[g_pw_try_index] = num;
		g_pw_try_index++;
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
	
	// motor girar 2 voltas no sentido anti-horario no modo meio passo

	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre fechado"));
}

void lock_safe()
{
	g_safe_is_locked++;

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Cofre travado"));
}

void blink_leds()
{
}

void toggle_light()
{
    if (g_light_is_on)
    {
        pinMode(PIN_LCD_LIGHT, OUTPUT);
        digitalWrite(PIN_LCD_LIGHT, LOW);
        g_light_is_on = 0;
    }
    else
    {
        pinMode(PIN_LCD_LIGHT, INPUT);
        g_light_is_on = 1;
    }
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
}

void handle_kb_press(const int8_t line, const int8_t col)
{
	char dig;
	
	if (line == 1)
	{
		if (col == 1)
			dig = 1;
		else if (col == 2)
			dig = 2;
		else if (col == 3)
			dig = 3;
		else if (col == 4)
			dig = -1;
	}
	else if (line == 2)
	{
		if (col == 1)
			dig = 4;
		else if (col == 2)
			dig = 5;
		else if (col == 3)
			dig = 6;
		else if (col == 4)
			dig = -1;
	} 
	else if (line == 3)
	{
		if (col == 1)
			dig = 7;
		else if (col == 2)
			dig = 8;
		else if (col == 3)
			dig = 9;
		else if (col == 4)
			dig = -1;
	} 
	else if (line == 4)
	{
		if (col == 1)
			dig = -1;
		else if (col == 2)
			dig = 0;
		else if (col == 3)
			dig = '#';
		else if (col == 4)
			dig = -1;
	}
	else if (line == -1)
		dig = BT_NONE;

	// Debounce
	if ((millis() - g_kb_delay) > DEBOUNCE_TIME*4)
	{
	    if ((dig == BT_NONE) && (g_prev_kb_state != BT_NONE))
	    {
	        if (g_prev_kb_state == '#')
	        	kb_hashtag_pressed();
	        else if (g_prev_kb_state >= 0 && g_prev_kb_state <= 9)
	        	kb_num_pressed(g_prev_kb_state);

	        g_kb_delay = millis();
	    }
	}
	g_prev_kb_state = dig;
}





void bt_left()
{
}

void bt_right()
{
}

void bt_up()
{
}

void bt_down()
{	
}

void bt_select()
{
}

void bt_released(int8_t bt)
{
    if (bt == BT_DOWN)
        bt_down();
    else if (bt == BT_UP)
        bt_up();
    else if (bt == BT_SELECT)
        bt_select();
    else if (bt == BT_LEFT)
        bt_left();
    else if (bt == BT_RIGHT)
        bt_right();
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
	// Debounce
    if ((millis() - g_bt_delay) > DEBOUNCE_TIME)
    {
        if ((bt == BT_NONE) && (g_prev_bt_state != BT_NONE))
        {
            bt_released(g_prev_bt_state);
            g_bt_delay = millis();
        }
    }
    g_prev_bt_state = bt;
}


void loop()
{
    const int8_t bt_pressed = check_bt_press();
    handle_bt_press(bt_pressed);

    check_kb_press();

    if (g_safe_is_locked)
    	blink_leds();
}
