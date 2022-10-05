#include "stm32f4xx.h"

/*
entrada: PC0 e PC2
saida: PB1, PB2, PB3, PB5, PB12, PB13, PB14, PB15
*/

/*
 * timer 10: piscar led em 1Hz (0,5s)
 * arr arbitrado: 1600 (1599)
 *
 * timer 11: pisca led em 3Hz (0,16666s)
 * arr arbitrado: 1600 (1599)
 */


/*
tim 10

0.5 = 1/16M * PSC * ARR
arr = 1600 arbitrado
psc = 0.5*16M/1600 = 5000



tim 11

0,16666 = 1/16M * PSC * ARR
arr = 1600 arbitrado
psc = 0,16666 * 16M/1600 = 1666,6 = 1667
*/

void liga(int pino){ // funcao liga pino x (numero 1 a 8)
	switch(pino){
	case 1:
		GPIOB->ODR|=0x00000002;  // liga o pino 1;
		break;
	case 2:
		GPIOB->ODR|=0x00000004;  // liga o pino 2;
		break;
	case 3:
		GPIOB->ODR|=0x00000008;  // liga o pino 3;
		break;
	case 4:
		GPIOB->ODR|=0x00000020;  // liga o pino 5;
		break;
	case 5:
		GPIOB->ODR|=0x00001000;  // liga o pino 12;
		break;
	case 6:
		GPIOB->ODR|=0x00002000;  // liga o pino 13;
		break;
	case 7:
		GPIOB->ODR|=0x00004000;  // liga o pino 14;
		break;
	case 8:
		GPIOB->ODR|=0x00008000;  // liga o pino 15;
		break;
	}
}

void desliga(int pino){ // funcao desliga pino x (numero 1 a 8)
	switch(pino){
	case 1:
		GPIOB->ODR&=~0x00000002;  // desliga o pino 1;
		break;
	case 2:
		GPIOB->ODR&=~0x00000004;  // desliga o pino 2;
		break;
	case 3:
		GPIOB->ODR&=~0x00000008;  // desliga o pino 3;
		break;
	case 4:
		GPIOB->ODR&=~0x00000020;  // desliga o pino 5;
		break;
	case 5:
		GPIOB->ODR&=~0x00001000;  // desliga o pino 12;
		break;
	case 6:
		GPIOB->ODR&=~0x00002000;  // desliga o pino 13;
		break;
	case 7:
		GPIOB->ODR&=~0x00004000;  // desliga o pino 14;
		break;
	case 8:
		GPIOB->ODR&=~0x00008000;  // desliga o pino 15;
		break;
	}
}

int pisca7(){ // pisca os 7 leds ate que botao 1 seja apertado
	while(1){
		for(int i = 1; i < 8; i++){
			liga(i);
		}

		while((TIM11->SR&0x01) == 0x00){
			if((GPIOC->IDR&=0x01)==0x01) return 0;
		}

		TIM11->SR&=~(0x01);

		for(int i = 1; i < 8; i++){
			desliga(i);
		}

		while((TIM11->SR&0x01) == 0x00){
			if((GPIOC->IDR&=0x01)==0x01) return 0;
		}

		TIM11->SR&=~(0x01);
	}

	return 0;
}

int contagemReg(){ // desliga os leds regressivamente até que botao 2 seja apertado


	for(int i = 0; i < 2; i++){ // delay 2*0,5s = 1s
		while((TIM10->SR&0x01) == 0x00){
			if((GPIOC->IDR&=0x04)==0x04) return 0; // retorna se o botao 2 foi apertado
		}

		TIM10->SR&=~(0x01);
	}


	for(int i = 1; i < 8; i++){ // desliga todos leds de 1 a 7, em ordem
		for(int i = 0; i < 2; i++){ // delay 2*0,5s = 1s
					while((TIM10->SR&0x01) == 0x00){
						if((GPIOC->IDR&=0x04)==0x04) return 0; // retorna se o botao 2 foi apertado
					}

					TIM10->SR&=~(0x01);
				}

		desliga(i);



	}

	liga(8);

	for(int i = 0; i < 6; i++){ // delay 6*0,5s = 3s
		while((TIM10->SR&0x01) == 0x00){
			if((GPIOC->IDR&=0x04)==0x04) return 0; // retorna se o botao 2 foi apertado
		}

		TIM10->SR&=~(0x01);
	}

	desliga(8);

	return 0;
}

int main(void)
{
	RCC->AHB1ENR=0x00000087;
	GPIOA->MODER=0x28000000;  // inicializa o modo debugger para a interface do ARM
	GPIOB->MODER=0x55000454;  // configura os pinos B1, B2, B3, B5, B12, B13, B14 e B15 com a função de saída
	GPIOC->MODER=0x00000000;  // configura os pinos C com função de entrada

	RCC->APB2ENR|=0x20000;
	RCC->APB2ENR|=0x40000;

	TIM10->ARR=1599;
	TIM10->PSC=4999;
	TIM10->CR1=0x05;

	TIM11->ARR=1599;
	TIM11->PSC=1666;
	TIM11->CR1=0x05;

	pisca7(); // pisca 7 leds ate botao 1 ser apertado


	for(int i = 1; i < 8; i++){ // liga os 7 leds
		liga(i);
	}

	contagemReg(); // inicia a contagem regressiva (para se botao 2 for apertado)
}
