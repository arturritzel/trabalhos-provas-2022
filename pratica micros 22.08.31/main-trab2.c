#include "stm32f4xx.h"

/*
 * piscar um led a 2,5hz e o outro a 6hz, parar de piscar se apertar botao
 * leds em PC0 e PC1 e botão em PB0
*/

int calculaArrEmFreq(float freq, float presc){
	float tempo = (1/freq)/2;
	return tempo*16000000/(presc+1);
}

int main(void)
{
	RCC->AHB1ENR=0x87;
	GPIOC->MODER=0x05; // PC0 e PC1 como saida
	GPIOB->MODER=0x00;  // PB0 como entrada

	RCC->APB2ENR|=0x20000;
	RCC->APB2ENR|=0x40000;

	TIM10->PSC=1599;
	TIM10->CR1=0x05;
	TIM10->ARR=calculaArrEmFreq(2.5, TIM10->PSC);

	TIM11->PSC=1599;
	TIM11->CR1=0x05;
	TIM11->ARR=calculaArrEmFreq(6, TIM11->PSC);

	int apertou = 0;

	while (1) {
		while(apertou == 1);
		if((GPIOB->IDR&=0x01)!=0x00) apertou = 1;

		if((TIM10->SR&0x01) == 0x01){
			GPIOC->ODR^=0x01;
			TIM10->SR&=~(0x01);
		}

		if((TIM11->SR&0x01) == 0x01){
			GPIOC->ODR^=0x02;
			TIM11->SR&=~(0x01);
		}






/*
while((TIM10->SR&0x01) == 0x00);
TIM10->SR&=~(0x01);
*/

	}
}
