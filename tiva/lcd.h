#ifndef LCD_H
#define LCD_H

#include <stdint.h>


// Limpa o Display
void LCD_Clear(void);

// Serve como um "clock"
void LCD_PulseEnable(void);

// Manda um byte pro processador do lcd
void LCD_SendByte(uint8_t byte, uint8_t rs);

// Move o cursor para o inicio da primeira linha
void LCD_Home(void);

// Define o cursor para uma posicao no grid 16x2
void LCD_SetCursor(uint8_t col, uint8_t row);

// Escreve caracter no display na posicao atual do cursor 
void LCD_WriteChar(char c);

// Escreve uma string a partir da posicao atual do cursor
void LCD_WriteString(char *str);

// Inicializa display o LCD
void LCD_Init(void);

#endif
