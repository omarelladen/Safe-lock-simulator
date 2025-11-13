#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTK  (0x200)  //bit 9
#define GPIO_PORTM  (0x800)  //bit 11

#define LCD_RS 0x01  // PM0
#define LCD_RW 0x02  // PM1
#define LCD_EN 0x04  // PM2

void SysTick_Wait1ms(uint32_t delay);


void LCD_PulseEnable(void){
    GPIO_PORTM_DATA_R |= LCD_EN;
    SysTick_Wait1ms(1);
    GPIO_PORTM_DATA_R &= ~LCD_EN;
    SysTick_Wait1ms(1);
}

void LCD_SendByte(uint8_t byte, uint8_t rs){
    if(rs) GPIO_PORTM_DATA_R |= LCD_RS;
    else   GPIO_PORTM_DATA_R &= ~LCD_RS;
    GPIO_PORTM_DATA_R &= ~LCD_RW;    // Escrita
    GPIO_PORTK_DATA_R = byte;        // Dados
    LCD_PulseEnable();
}

void LCD_Clear(void){
    LCD_SendByte(0x01,0);
    SysTick_Wait1ms(5);
}

void LCD_Home(void){
    LCD_SendByte(0x02,0);
    SysTick_Wait1ms(5);
}

void LCD_SetCursor(uint8_t col, uint8_t row){
    uint8_t address = (col == 0) ? 0x00 : 0x40;
    address += row;
    LCD_SendByte(0x80 | address,0);
}

void LCD_WriteChar(char c){
    LCD_SendByte(c,1);
}


void LCD_WriteString(char *str){
    while(*str){
        LCD_WriteChar(*str++);
    }
}


void LCD_Init(void){
    // Habilita clock Port K e M
    SYSCTL_RCGCGPIO_R |= (GPIO_PORTM) | (GPIO_PORTK);
    while((SYSCTL_PRGPIO_R & (GPIO_PORTK)) == 0);
    while((SYSCTL_PRGPIO_R & (GPIO_PORTM)) == 0);

    // Configura PK0-PK7 como saída digital
    GPIO_PORTK_DIR_R |= 0xFF;
    GPIO_PORTK_DEN_R |= 0xFF;

    // Configura PM0-PM2 como saída digital
    GPIO_PORTM_DIR_R |= 0x07;
    GPIO_PORTM_DEN_R |= 0x07;

    SysTick_Wait1ms(20);
    LCD_SendByte(0x38,0);
		SysTick_Wait1ms(5);	// 8 bits, 2 linhas, 5x8 dots
    LCD_SendByte(0x0C,0); // Display on, cursor off
    SysTick_Wait1ms(5);
		LCD_SendByte(0x06,0); // Incrementa cursor
		SysTick_Wait1ms(5);
		LCD_Clear();
}
