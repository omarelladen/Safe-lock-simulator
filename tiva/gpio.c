// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

#define GPIO_PORTA  (0x0001) //bit 0
#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTK  (0x200)  //bit 9
#define GPIO_PORTL  (0x400)  //bit 10
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTM  (0x800)  //bit 11
#define GPIO_PORTQ  (0x4000) //bit 14

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTM | GPIO_PORTL);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTM | GPIO_PORTL) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTM | GPIO_PORTL) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
  GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTA_AHB_DIR_R = 0x0F;
	GPIO_PORTQ_DIR_R = 0x0F;
	GPIO_PORTM_DIR_R = 0x07;
	GPIO_PORTL_DIR_R = 0x00;
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTA_AHB_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00; 	
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
	
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTJ_AHB_DEN_R = 0x03;   //Bit0 e bit1
	GPIO_PORTN_DEN_R = 0x03; 		   //Bit0 e bit1
	GPIO_PORTA_AHB_DEN_R = 0x0F;   //Bit0 e bit1
	GPIO_PORTQ_DEN_R = 0x0F;			 //Bit0 e bit1
	GPIO_PORTM_DEN_R = 0xF7;
	GPIO_PORTL_DEN_R = 0x0F;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;	//Bit0 e bit1
	GPIO_PORTL_PUR_R = 0x0F;
	
	

}	

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortN_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTN_DATA_R = temp; 
}




