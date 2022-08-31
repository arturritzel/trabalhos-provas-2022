#include "stm32f4xx.h"

/*
 * ao pressionar um botao, o led pisca com periodo de 0,4s por 5s.
 * para de piscar por 2seg e dps volta a piscar por 5 seg.
 * botao em b e led em c
 */

/*
 * tempo = freq do arm * psc * arr
 * tempo*16m/psc = arr
 * 0.1s*16000000/1600 = 10000
*/

int main(void)
{
	RCC->AHB1ENR=0x87;
	GPIOC->MODER=0x01; // PC0 como saida
	GPIOB->MODER=0x00;  // PB0 como entrada

	RCC->APB2ENR|=0x20000;
	RCC->APB2ENR|=0x40000;

	TIM10->PSC=1599;
	TIM10->CR1=0x05;
	TIM10->ARR=1000;

	while (1) {
		while((GPIOB->IDR&=0x01)!=0x01);

		// cada ciclo será 10 tempos de 0,1s, totalizando 1s
		// entao um ciclo de 50 tempos seriam 5s
		for(int i = 0; i < 50; i++){
			if(i % 2 == 0) GPIOC->ODR^=0x01;

			while((TIM10->SR&0x01) == 0x00);
			TIM10->SR&=~(0x01);
		}
		GPIOC->ODR=0x00;

		for(int i = 0; i < 20; i++){
			while((TIM10->SR&0x01) == 0x00);
			TIM10->SR&=~(0x01);
		}

	for(int i = 0; i < 50; i++){
		if(i % 2 == 0) GPIOC->ODR^=0x01;

		while((TIM10->SR&0x01) == 0x00);
		TIM10->SR&=~(0x01);
	}
	GPIOC->ODR=0x00;

	}
}
