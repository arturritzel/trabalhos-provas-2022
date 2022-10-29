#include "stm32f4xx.h"

/*
 *  codigo pra testar as ligacoes nos displays
 *  idealmente sao 4 displays com alimentacao em 3v3
 *  PC0 no pino A de todos os displays, PC1 no pino B, PC2 no C, PC3 no D, e assim por diante, até PC6 no pino G
 */


/*
enum letrasAlfabeto{
	letraA, letraB, letraC, letraD, letraE, letraF, letraG, letraH, letraI, letraJ,
	letraK, letraL, letraM, letraN, letraO, letraP, letraQ, letraR, letraS, letraT,
	letraU, letraV, letraW, letraX, letraY, letraZ
};


int numero7seg[10]={
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
}; // */

int letra7seg[26]={ // declara as letras em hexa (letra7seg[0] = A, [1] = B, [2] = C, etc.
	0x77, 0x7C, 0x58, 0x5E, 0x79,
	0x71, 0x6F, 0x76, 0x06, 0x0E,
	0x70, 0x38, 0x55, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x6D, 0x78,
	0x3E, 0x1C, 0x1D, 0x76, 0x66,
	0x5B
};

int valorDisplay[4] = {0, 0, 0, 0};

int contagem = 0;

void TIM1_TRG_COM_TIM11_IRQHandler(){
	GPIOC->ODR = ~letra7seg[contagem];

	//contagem++;
	//if(contagem >= 26) contagem = 0;

	TIM11->SR &= ~(0x01);    //limpando flag do timer11
}

int main(){

	RCC->AHB1ENR=0x00000087;  // habilita os contadores
	GPIOA->MODER=0x28000000;  // inicializa o modo debugger para a interface do ARM
	GPIOB->MODER=0x00000055;  // configura os pinos B1, B2, B3, B4
	GPIOC->MODER=0x00555555;  // configura PC0-PC11 com função de saída e PC12-PC15 com função de entrada

	GPIOC->ODR = 0; // limpa as saidas da porta C

	RCC->APB2ENR|=0x20000; // habilita clock tim10
	RCC->APB2ENR|=0x40000; // habilita clock tim11

	TIM10->PSC=1599; // troca o display q ta ligado (0,25s) //////////////////////////////// tem q mudar pra mais de 60 Hz (0,015s = 66,66...Hz)
	TIM10->CR1=0x05;
	TIM10->ARR=2499;
	TIM10->DIER|=0x01; //habilita interrupção timer10
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 4); // prioridade 4
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn); // habilita interrupção no controlador

	TIM11->PSC=159; // muda oq tem no display (0,5s) ////////////////// colocar 9 no final
	TIM11->CR1=0x05;
	TIM11->ARR=4999;
	TIM11->DIER|=0x01; //habilita interrupção timer11
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 4); // prioridade 4
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn); // habilita interrupção no controlador

	while(1){
		if((GPIOC->IDR&=0x2000)!=0x2000){
			botao++;
		}

		if(botao > 150000){
			contagem++;
			if(contagem >= 26) contagem = 0;
			botao = 0;
		}
	}
}


















