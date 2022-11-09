#include "stm32f4xx.h"

/*
 *  codigo pra display de uma frase
 *  idealmente 4 displays multiplexados com alimentacao em 3v3 (PB0, PB1, PB2, PC3)
 *  PC0 no pino A de todos os displays, PC1 no pino B, PC2 no C, PC3 no D, e assim por diante, até PC6 no pino G
 */

//#define tamanho 6

char frase[] = "multiplexacao com displays";



int numero[10]={
		0x3F, // 0
		0x06, // 1
		0x5B, // 2
		0x4F, // 3
		0x66, // 4
		0x6D, // 5
		0x7D, // 6
		0x27, // 7
		0x7F, // 8
		0x6F  // 9
};

char defletras[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

int letra[26]={ // declara as letras em hexa (letra7seg[0] = A, [1] = B, [2] = C, etc.
	0x77, 0x7C, 0x58, 0x5E, 0x79,
	0x71, 0x6F, 0x76, 0x06, 0x0E,
	0x70, 0x38, 0x55, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x6D, 0x78,
	0x3E, 0x1C, 0x1D, 0x76, 0x66,
	0x5B
};

int getHexa(char digito){
	for(int i = 0; i < 26; i++){
		if(digito == defletras[i]){
			return letra[i];
		}
	}

	return 0;
}

int qualDisplay = 0;
int valorDisplay[4] = {0, 0, 0, 0};

int contagem = 0;

void TIM1_UP_TIM10_IRQHandler(){
	switch(qualDisplay){
	    case 0:
	        GPIOC->ODR = ~valorDisplay[qualDisplay];
	        GPIOB->ODR = 0x1;
	        break;

	    case 1:
	        GPIOC->ODR = ~valorDisplay[qualDisplay];
	        GPIOB->ODR = 0x2;
	        break;

	    case 2:
	        GPIOC->ODR = ~valorDisplay[qualDisplay];
	        GPIOB->ODR = 0x4;
	        break;

	    case 3:
	    	GPIOC->ODR = ~valorDisplay[qualDisplay];
	        GPIOB->ODR = 0x8;
	        break;
	    }

	qualDisplay++;
	if(qualDisplay > 3) qualDisplay = 0;

	TIM10->SR &= ~(0x01);    //limpando flag do timer10
}

void TIM1_TRG_COM_TIM11_IRQHandler(){


	if(contagem >= ((sizeof(frase)/sizeof(frase[0])+2)))
		//return; // se quiser que pare a mensagem
		contagem = 0; // se quiser que volte a mensagem

	valorDisplay[3] = valorDisplay[2];
	valorDisplay[2] = valorDisplay[1];
	valorDisplay[1] = valorDisplay[0];
	valorDisplay[0] = getHexa(frase[contagem]);

	contagem++;


	TIM11->SR &= ~(0x01);    //limpando flag do timer11
}

int main(){

	RCC->AHB1ENR=0x00000087;  // habilita os contadores
	GPIOA->MODER=0x28000000;  // inicializa o modo debugger para a interface do ARM
	GPIOB->MODER=0x00000055;  // configura os pinos B0, B1, B2, B3 como saida
	GPIOC->MODER=0x00555555;  // configura PC0-PC11 com função de saída e PC12-PC15 com função de entrada

	GPIOC->ODR = 0; // limpa as saidas da porta C
	GPIOB->ODR = 0; // limpa as saidas da porta C

	RCC->APB2ENR|=0x20000; // habilita clock tim10
	RCC->APB2ENR|=0x40000; // habilita clock tim11

	TIM10->PSC=249; // troca o display q ta ligado (4ms) //////////////// precisa ser mais de 60*4 Hz (menos de 0,004167s)
	TIM10->CR1=0x05;
	TIM10->ARR=249;
	TIM10->DIER|=0x01; //habilita interrupção timer10
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 4); // prioridade 4
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); // habilita interrupção no controlador

	TIM11->PSC=1599; // muda oq tem no display (0,5s)
	TIM11->CR1=0x05;
	TIM11->ARR=4999;
	TIM11->DIER|=0x01; //habilita interrupção timer11
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 3); // prioridade 3
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn); // habilita interrupção no controlador


	/*
	valorDisplay[0] = letra[letraV];
	valorDisplay[1] = letra[letraA];
	valorDisplay[2] = letra[letraN];
	valorDisplay[3] = letra[letraA]; // */

	while(1){

	}
}
