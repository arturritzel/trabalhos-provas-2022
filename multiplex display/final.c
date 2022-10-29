#include "stm32f4xx.h"

/*
 *  codigo pra display de uma frase
 *  idealmente 4 displays multiplexados com alimentacao em 3v3 (PB0, PB1, PB2, PC3)
 *  PC0 no pino A de todos os displays, PC1 no pino B, PC2 no C, PC3 no D, e assim por diante, até PC6 no pino G
 */

//#define tamanho 6

char frase[] = "MULTIPLEXACAO COM 4 DISPLAYS";

enum letrasAlfabeto{
	letraA, letraB, letraC, letraD, letraE, letraF, letraG, letraH, letraI, letraJ,
	letraK, letraL, letraM, letraN, letraO, letraP, letraQ, letraR, letraS, letraT,
	letraU, letraV, letraW, letraX, letraY, letraZ
};

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

int letra[26]={ // declara as letras em hexa (letra7seg[0] = A, [1] = B, [2] = C, etc.
	0x77, 0x7C, 0x58, 0x5E, 0x79,
	0x71, 0x6F, 0x76, 0x06, 0x0E,
	0x70, 0x38, 0x55, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x6D, 0x78,
	0x3E, 0x1C, 0x1D, 0x76, 0x66,
	0x5B
};

int getHexa(char digito){
	if(digito == 'A') return letra[letraA];
    if(digito == 'B') return letra[letraB];
   	if(digito == 'C') return letra[letraC];
	if(digito == 'D') return letra[letraD];
	if(digito == 'E') return letra[letraE];
	if(digito == 'F') return letra[letraF];
	if(digito == 'G') return letra[letraG];
	if(digito == 'H') return letra[letraH];
	if(digito == 'I') return letra[letraI];
	if(digito == 'J') return letra[letraJ];
	if(digito == 'K') return letra[letraK];
	if(digito == 'L') return letra[letraL];
	if(digito == 'M') return letra[letraM];
	if(digito == 'N') return letra[letraN];
	if(digito == 'O') return letra[letraO];
	if(digito == 'P') return letra[letraP];
	if(digito == 'Q') return letra[letraQ];
	if(digito == 'R') return letra[letraR];
	if(digito == 'S') return letra[letraS];
	if(digito == 'T') return letra[letraT];
	if(digito == 'U') return letra[letraU];
	if(digito == 'V') return letra[letraV];
	if(digito == 'W') return letra[letraW];
	if(digito == 'X') return letra[letraX];
	if(digito == 'Y') return letra[letraY];
	if(digito == 'Z') return letra[letraZ];

	if(digito == '1') return numero[1];
	if(digito == '2') return numero[2];
	if(digito == '3') return numero[3];
	if(digito == '4') return numero[4];
	if(digito == '5') return numero[5];
	if(digito == '6') return numero[6];
	if(digito == '7') return numero[7];
	if(digito == '8') return numero[8];
	if(digito == '9') return numero[9];
	if(digito == '0') return numero[0];

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


	if(contagem >= ((sizeof(frase)/sizeof(frase[0])+4)))
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

	TIM10->PSC=249; // troca o display q ta ligado (4ms) //////////////// precisa ser mais de 60*4 Hz (menos de 0,004s)
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
