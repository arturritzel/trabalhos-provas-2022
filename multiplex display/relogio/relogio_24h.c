#include "stm32f4xx.h"

/*
 *  codigo pra display de um relogio formato 00:00:00 utilizando apenas 4 displays.
 *  a cada unidade de tempo (idealmente 0,5 segundos), os digitos de cada display X se tornam o digito anterior do display X+1,
 *  dando a impressao de que os numeros estão indo da esquerda para a direita.
 *
 *  idealmente 4 displays multiplexados com alimentacao em 3v3 (PB0, PB1, PB2, PB3)
 *  PC0 no pino A de todos os displays, PC1 no pino B, PC2 no C, PC3 no D, e assim por diante, até PC6 no pino G
 *  botões em PA0, PA1 e PA4, que aumentam hora, minuto e segundo, respectivamente.
 */

int numero[11]={
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
		0x00  // 10 (indef)
};

int qualDisplay = 0, contagem = 0, contaSeg = 0;

int valorDisplay[4] = {0, 0, 0, 0};

// hora, hora, minuto, minuto, segundo, segundo, carac indefinido
int horario[7] = {1, 0, 3, 0, 0, 0, 10};

void adicionaHora(){ // funcao para adicionar hora
	horario[1]++; // aumenta 1 na unidade de hora
	if(horario[1] > 9){ // caso a unidade seja maior que 9
		horario[1] = 0; // torna a unidade 0
		horario[0]++; // aumenta a dezena de hora

	}

	if(horario[0] > 1 && horario[1] > 3){
		horario[0] = 0; // reseta caso passe das 24 horas
		horario[1] = 0;
	}
}

void adicionaMin(){ // aumentar minuto
	horario[3]++;
	if(horario[3] > 9){ // caso a unidade seja maior que 9
		horario[3] = 0; // zera a unidade
		horario[2]++; // aumenta a dezena

		if(horario[2] > 5){ // caso a dezena de minuto seja maior que 5
			horario[2] = 0; // zera os minutos

			adicionaHora(); // aumenta a hora
		}
	}
}

void adicionaSeg(){ // aumenta segundo
	horario[5]++;
	if(horario[5] > 9){ // caso a unidade seja maior que 9
		horario[5] = 0; // zera a unidade
		horario[4]++; // aumenta a dezena

		if(horario[4] > 5){ // caso a dezena de segundos seja maior que 5
			horario[4] = 0; // zera os segundos

			adicionaMin(); // aumenta minuto
		}
	}
}

// predefinindo o formato de rotacao como HH_MM_SS__ (espaços entre digitos e dois espaços sinalizando o final)
// hora, hora, indef, minuto, minuto, indef, seg, seg, indef, indef
int rotacao[11] = {0, 1, 6, 2, 3, 6, 4, 5, 6, 6, 6};

void TIM1_UP_TIM10_IRQHandler(){

	// calcula o valor de cada display
	/*
	 *  1- pega um numero de 0 a 10 (qntd de caracteres desejados na rotacao)
	 *  2- verifica qual posicao do vetor horario deve ser usada pelo numero escolhido
	 *  3- verifica como mostrar em hexa o valor recebido
	 */
	valorDisplay[3] = numero[horario[rotacao[(contagem+0)%11]]];
	valorDisplay[2] = numero[horario[rotacao[(contagem+1)%11]]];
	valorDisplay[1] = numero[horario[rotacao[(contagem+2)%11]]];
	valorDisplay[0] = numero[horario[rotacao[(contagem+3)%11]]];

	// qual display deve ser ligado
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

	contaSeg++;
	if(contaSeg >= 2){ // verifica se ja passou um segundo (2 ciclos de 0,5 seg)
		contaSeg = 0;
		adicionaSeg();
	}

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

	TIM11->PSC=3999; // muda oq tem no display (0,5s)
	TIM11->CR1=0x05;
	TIM11->ARR=1999;
	TIM11->DIER|=0x01; //habilita interrupção timer11
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 3); // prioridade 3
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn); // habilita interrupção no controlador

	int apertou[3] = {0, 0, 0};
	int apertando[3] = {0, 0, 0};

	while(1){
		if((GPIOA->IDR&=0x01)==0x01 && apertou[3] == 0) apertando[3]++; // caso pa0 esteja sendo apertado
		if((GPIOA->IDR&=0x02)==0x02 && apertou[2] == 0) apertando[2]++; // ==== pa1 =====================
		if((GPIOA->IDR&=0x10)==0x10 && apertou[1] == 0) apertando[1]++; // ==== pa4 =====================

		if((GPIOA->IDR&=0x01)!=0x01 && apertou[3] == 1) apertou[3] = 0; // caso pa0 nao esteja sendo apertado
		if((GPIOA->IDR&=0x02)!=0x02 && apertou[2] == 1) apertou[2] = 0; // ==== pa1 =========================
		if((GPIOA->IDR&=0x10)!=0x10 && apertou[1] == 1) apertou[1] = 0; // ==== pa4 =========================

		if(apertando[1] > 10000){
			apertou[1] = 1;
			apertando[1] = 0;
			adicionaSeg();
		}

		if(apertando[2] > 10000){
			apertou[2] = 1;
			apertando[2] = 0;
			adicionaMin();
		}

		if(apertando[3] > 10000){
			apertou[3] = 1;
			apertando[3] = 0;
			adicionaHora();
		}

	}
}
