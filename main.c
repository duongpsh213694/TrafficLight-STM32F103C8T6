#include <stdint.h>
#include "stm32f10x.h"
#include "lcd_1602_drive.h"
#include <stdio.h>
/*#include"Systick.h"*/

/*Clock*/
#define Flash_ACR   (uint32_t*)0x40022000
#define RCC_CR		  (uint32_t*)0x40021000
#define RCC_CFGR    (uint32_t*)0x40021004
#define RCC_APB2ENR (uint32_t*)0x40021018
/* Systick*/
#define ST_CTRL			(uint32_t*)0xE000E010
#define ST_RELOAD   (uint32_t*)0xE000E014
#define ST_VAL      (uint32_t*)0xE000E018

/*Port A*/
#define GPIOA_CRL (uint32_t*)0x40010800
#define GPIOA_CRH (uint32_t*)0x40010804
#define GPIOA_IDR (uint32_t*)0x40010808

/*Port B*/
#define GPIOB_CRL (uint32_t*)0x40010C00
#define GPIOB_CRH (uint32_t*)0x40010C04
#define GPIOB_ODR (uint32_t*)0x40010C0C

/* Interrupt Registers*/
#define AFIO_EXTICR2 (uint32_t*)0x4001000C
#define EXTI_IMR		 (uint32_t*)0x40010400
#define EXTI_RTSR		 (uint32_t*)0x40010408
#define EXTI_FTSR		 (uint32_t*)0x4001040C
#define EXTI_PR		 	 (uint32_t*)0x40010414
#define NVIC_ISER0	 (uint32_t*)0xE000E100

#define GoS     0
#define WaitS   1
#define StopS   2
#define GoW     3
#define WaitW   4
#define StopW   5
#define Walk    6
#define On1     7
#define Off1    8
#define On2     9
#define Off2    10
#define NoWalk  11
#define StopAll 12

#define GoTime    1000
#define WaitTime  500 
#define StopTime  50
#define blinkTime 50

struct state{
	uint32_t output;
	uint32_t time;
	uint32_t next[8];
};

typedef const struct state st;

static st FSM[13]={
		/*GoS*/
		{0x8042, GoTime, {WaitS,	WaitS,	GoS,	WaitS,	WaitS,	WaitS,	WaitS,	WaitS}},
		/*WaitS*/
		{0x8082, WaitTime, {StopAll,	StopAll,	StopAll,	StopS,	StopAll,	StopAll,	StopS,	StopS}},
		/*StopS*/
		{0x8102, StopTime, {StopAll,	GoW,	GoS,	GoW,	Walk,	GoW,	Walk,	GoW}},
		/*GoW*/
		{0x1102, GoTime, {WaitW,	GoW,	WaitW,	WaitW,	WaitW,	WaitW,	WaitW,	WaitW}},
		/*WaitW*/
		{0x2102, WaitTime, {StopAll,	StopAll,	StopAll,	StopW,	StopAll,	StopW,	StopAll,	StopW}},
		/*StopW*/
		{0x8102, StopTime, {StopAll,	GoW,	GoS,	GoS,	Walk,	Walk,	Walk,	Walk}},
		/*Walk*/
		{0x8101, GoTime, {On1,	On1,	On1,	On1,	Walk,	On1,	On1,	On1}},
		/*On1*/
		{0x8102, blinkTime, {Off1,	Off1,	Off1,	Off1,	Off1,	Off1,	Off1,	Off1}},
		/*Off1*/
		{0x8100, blinkTime, {On2,	On2,	On2,	On2,	On2,	On2,	On2,	On2}},
		/*On2*/
		{0x8102, blinkTime, {Off2,	Off2,	Off2,	Off2,	Off2,	Off2,	Off2,	Off2}},
		/*Off2*/
		{0x8100, blinkTime, {StopAll,	StopAll,	StopAll,	StopAll,	NoWalk,	NoWalk,	NoWalk,	NoWalk}},
		/*NoWalk*/
		{0x8102, StopTime,{StopAll,	GoW,	GoS,	GoS,	Walk,	GoW,	GoS,	GoS}},
		/*StopAll*/
		{0x8102, StopTime,{StopAll,	GoW,	GoS,	GoS,	Walk,	GoW,	GoS,	GoS}}
};

static uint32_t input = 0x00;
static uint32_t lastInput = 0x00;
static uint32_t counter = 0;
static uint32_t RedAll;
static uint32_t GreenS;
static uint32_t GreenW;
static uint32_t GreenWalk;

/* PA7,6,5 for input: Walk South West */
static void portA_init(void){
	/* Set PA7,6,5 as input*/
	*(GPIOA_CRL) &= 0x00000000;
	*(GPIOA_CRL) |= 0x88800000;
	/* Turn on clock for AFIO*/
  *(RCC_APB2ENR) |= 0x01;

	/* Configure interrupt for PA7,6,5*/
  *(AFIO_EXTICR2) &= !(0x00000F00); 
	*(AFIO_EXTICR2) &= 0x00000000;      

  *(EXTI_FTSR) |= 0x00000020 | 0x00000040 | 0x00000080;
  *(EXTI_RTSR) |= 0x00000020 | 0x00000040 | 0x00000080;

  *(EXTI_IMR) |= 0x00000020 | 0x00000040 | 0x00000080;
		
	/*Enable interrupt*/
  *(NVIC_ISER0) |= 1<<23;
		
}

/* PB15,13,12,8,7,6,1,0 for output for West(R Y G) South(R Y G) Walk(R G)*/
static void portB_init(void){
	*(GPIOB_CRH) &= 0x00000000;
	*(GPIOB_CRL) &= 0x00000000;
	*(GPIOB_ODR) &= 0x00000000;
	*(GPIOB_CRH) |= 0x10110001;
	*(GPIOB_CRL) |= 0x11000011;
}

static void system_init(void){
	/* Enable HSE*/
	*(RCC_CR) |= (1 << 16);
	/* Waiting for HSE to be ready*/
	while(!( (*RCC_CR) & (1<<17) ));
	/* Flash buffer*/
	*(Flash_ACR) |= (1<<4);
	/* Latency*/
	*(Flash_ACR) |= 0x2;
	/* PLLXTPRE PLLSRC PLLMUL SW*/
	/* Clear bits*/
	*(RCC_CFGR) &= (0<<16);
	*(RCC_CFGR) &= (0<<17);
	*(RCC_CFGR) &= (0<<18);
	*(RCC_CFGR) &= (0<<19);
	*(RCC_CFGR) &= (0<<20);
	*(RCC_CFGR) &= (0<<21);
	/* Configure bits*/
	*(RCC_CFGR) &= (0<<17);
	*(RCC_CFGR) |= (1<<16);
	*(RCC_CFGR) |= 0x000C0000;
	*(RCC_CFGR) &= (0<<7);
	*(RCC_CFGR) &= (0<<13);
	*(RCC_CFGR) &= (0<<10);
	/* Turn on PLL*/
	*(RCC_CR) 	|= (1<<24);
	while( !(*(RCC_CR) & (1<<25)) );
	/* Set PLL as clock source*/
	*(RCC_CFGR) &= 0x0;
	*(RCC_CFGR) |= 0x2;
	while( !(*(RCC_CFGR) & (2<<2)) );
}

/* Systick init*/
static void Systick_init(void){
	*(ST_CTRL) &= 0;
	*(ST_CTRL) |= 0x05;
}

/* Delay for 10ms if time = 720000*/
static void delay10ms(uint32_t time){
	*(ST_RELOAD) = time - 1;
	*(ST_VAL) = 0;
	while((*(ST_CTRL) & 0x00010000)==0){ /* wait for count flag */}
}

/* Delay for time/100 second*/
static void delay(uint32_t time){
	  volatile uint32_t i;
		char counter_str[3];
		if(!(GreenS || GreenW || GreenWalk)){
			counter = 0;
		}
		for(i = 1; i <= time; i++){
			if(input != lastInput && (GreenS || GreenW || GreenWalk) && (counter>10)){
				break;
			}
			delay10ms(720000); /* delay 10ms*/
			if(RedAll == 0x8100){
				counter = 0;
			}
			if(i % 100 == 0){ /* i = 100 means 1 second*/
				counter++;
			}
			sprintf(counter_str, "%02d", counter);
			lcd_i2c_msg(2, 1, 0, counter_str);
		}
}
void EXTI9_5_IRQHandler(void);

int main(void)
{
	/* Current state*/
	uint32_t cs;
	
	/* Systick init*/
	Systick_init();
	/* Enable clock*/
	*(RCC_APB2ENR) |= 0x0C;
	
	/* Initialize GPIO*/
	portA_init();
	portB_init();
	
	/* Turn on LCD*/
	lcd_i2c_init(2);
	lcd_i2c_msg(2, 1, 0, "00");
	
	/* Current state = StopAll*/
	cs = StopAll;
	
	/* While loop*/
	while(1){
		/* Update flags*/
		RedAll = FSM[cs].output;
		GreenS = (FSM[cs].output & 0x0040) >> 6;
		GreenW = (FSM[cs].output & 0x1000) >> 12;
		GreenWalk = (FSM[cs].output & 0x0001);
		
		lastInput = input;
		/* LED output*/
		*(GPIOB_ODR) = FSM[cs].output;
		/* Delay*/
		delay(FSM[cs].time);
		/* Check input*/
		input = (*(GPIOA_IDR) & 0x00E0) >> 5;
		/* Update current state*/
		cs = FSM[cs].next[input];
	}
}

/* Interrupt Handler*/
void EXTI9_5_IRQHandler(void){
	 if (*(EXTI_PR) & (1<<5)) {  
		 *(EXTI_PR) |= (1<<5);
    }

    if (*(EXTI_PR) & (1<<6)) {
			*(EXTI_PR) |= (1<<6); 
    }

    if (*(EXTI_PR) & (1<<7)) {
			*(EXTI_PR) |= (1<<7);
    }
	input = (*(GPIOA_IDR) & 0x00E0) >> 5;
}
