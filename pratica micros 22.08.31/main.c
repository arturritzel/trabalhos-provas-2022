#include "stm32f4xx.h"

/*
 * ao pressionar um botao, o led pisca com periodo de 0,4s por 5s.
 * para de piscar por 2seg e dps volta a piscar por 5 seg.
 * botao em b e led em c
 */

/*
 * tempo = freq do arm * psc * arr
 * tempo*16m/psc = arr
*/

int calculaArrEmSeg(float tempo, float presc){ // tempo em segundos
	return tempo*16000000/presc;
}

void piscaPor(float tempoPiscada, float tempoPiscando){ // tempo em segundos
	if(tempoPiscada == 0)
		GPIOC->ODR=0x00;
		TIM11->ARR=calculaArrEmSeg(tempoPiscando, TIM11->PSC);
		while((TIM11->SR&0x01) == 0x00);
		TIM11->SR&=~(0x01);

	if(tempoPiscada != 0){
	TIM10->ARR=calculaArrEmSeg(tempoPiscada, TIM10->PSC);
	TIM11->ARR=calculaArrEmSeg(tempoPiscando, TIM11->PSC);

	while((TIM11->SR&0x01) == 0x00){

		if((TIM10->SR&0x01) == 0x01){
		TIM10->SR&=~(0x01);
		GPIOC->ODR^=0x01;
		}
	}
	GPIOC->ODR=0x00;
	TIM11->SR&=~(0x01);
	}
}

int main(void)
{
	RCC->AHB1ENR=0x87;
	GPIOC->MODER=0x01; // PC0 como saida
	GPIOB->MODER=0x00;  // PB0 como entrada

	RCC->APB2ENR|=0x20000;
	RCC->APB2ENR|=0x40000;

	TIM10->PSC=1599;
	TIM10->CR1=0x05;
	TIM10->ARR=100;

	TIM11->PSC=1599;
	TIM11->CR1=0x05;
	TIM11->ARR=100;

	while (1) {
		while((GPIOB->IDR&=0x01)!=0x01);


		piscaPor(0.2, 5);

		TIM11->ARR = 20000;
		while((TIM11->SR&0x01) == 0x00);
		TIM11->SR&=~(0x01);

		piscaPor(0.2, 5);

	}
}
