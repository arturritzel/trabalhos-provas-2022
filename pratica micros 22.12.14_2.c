#include "stm32f4xx.h"

/*
 * Multiplexação de Displays:
 * contador hexadecimal de 2 dígitos
 * apertar o botão em PB0 faz o número diminuir, apertar o botão em PB1 faz o número aumentar
 * alimentação dos displays em PA0 e PA1; entradas a-g dos displays em PC0-PC6
 * 
 */

int numero[16]={
		0x3F, // 0
		0x06, // 1
		0x5B, // 2
		0x4F, // 3
		0x66, // 4
		0x6D, // 5
		0x7D, // 6
		0x27, // 7
		0x7F, // 8
		0x6F, // 9
		0x77, // A
		0x7C, // B
		0x58, // C
		0x5E, // D
		0x79, // E
		0x71  // F
};

int qualDisplay = 0;

int valorDisplay[2] = {0, 0};

void adicionaSeg(){ // aumenta segundo
	valorDisplay[1]++;
	if(valorDisplay[1] > 15){
		valorDisplay[1] = 0;
		valorDisplay[0]++;
		if(valorDisplay[0] > 15)
			valorDisplay[0] = 0;
	}
}

void removeSeg(){ // aumenta segundo
	valorDisplay[1]--;
	if(valorDisplay[1] < 0){
		valorDisplay[1] = 15;
		valorDisplay[0]--;
		if(valorDisplay[0] < 0)
			valorDisplay[0] = 15;
	}
}

void TIM1_UP_TIM10_IRQHandler(){

	switch(qualDisplay){
	    case 0:
	        GPIOC->ODR = ~numero[valorDisplay[qualDisplay]];
	        GPIOA->ODR = 0x1;
	        break;

	    case 1:
	        GPIOC->ODR = ~numero[valorDisplay[qualDisplay]];
	        GPIOA->ODR = 0x2;
	        break;
	    }

	qualDisplay++;
	if(qualDisplay > 1) qualDisplay = 0;

	TIM10->SR &= ~(0x01);    //limpando flag do timer10
}

int main(){

	RCC->AHB1ENR=0x00000087;  // habilita os contadores
	GPIOA->MODER=0x28000005;  // configura A0 e A1 como saída
	GPIOB->MODER=0x00000555;  // configura os pinos B0, B1, B2, B3, B4, B5 como saida
	GPIOC->MODER=0x00555555;  // configura PC0-PC11 com função de saída e PC12-PC15 com função de entrada

	GPIOC->ODR = 0; // limpa as saidas da porta C
	GPIOB->ODR = 0; // limpa as saidas da porta B

	RCC->APB2ENR|=0x20000; // habilita clock tim10
	RCC->APB2ENR|=0x40000; // habilita clock tim11

	TIM10->PSC=99; // troca o display q ta ligado (1.5ms) //////////////// precisa ser mais de 60*4 Hz (menos de 0,004167s) e mais que 1kHz pra ficar legal o som do alarme
	TIM10->CR1=0x05;
	TIM10->ARR=99;
	TIM10->DIER|=0x01; //habilita interrupção timer10
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 4); // prioridade 4
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); // habilita interrupção no controlador

	int apertando[2] = {0,0}, apertou[2] = {0,0};
	while(1){

		if((GPIOB->IDR&=0x01)==0x01 && apertou[0] == 0) apertando[0]++; // caso pb0 esteja sendo apertado
		if((GPIOB->IDR&=0x01)!=0x01 && apertou[0] == 1) apertou[0] = 0; // caso pb0 nao esteja sendo apertado

		if((GPIOB->IDR&=0x02)==0x02 && apertou[1] == 0) apertando[1]++; // caso pb1 esteja sendo apertado
		if((GPIOB->IDR&=0x02)!=0x02 && apertou[1] == 1) apertou[1] = 0; // caso pb1 nao esteja sendo apertado

		if(apertando[0] > 20000){ // pb0
			apertou[0] = 1;
			apertando[0] = 0;

			adicionaSeg();
		}

		if(apertando[1] > 20000){ // pb1
			apertou[1] = 1;
			apertando[1] = 0;

			removeSeg();
		}
	}
}
