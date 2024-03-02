/*
 * Register.h
 *
 *  Created on: Jan 11, 2024
 *      Author: TBTHANH
 *     Purpose: To create a memory map structure of GPIOA,B and RCC
 */
#include<stdint.h>
#include"Register.h"

#ifndef REGISTER_H_
#define REGISTER_H_

/* Register address */

#define RCC_BASE			((uint32_t *)(0x40021000))
#define GPIO_A_BASE			((uint32_t *)(0x40010800))
#define GPIO_B_BASE			((uint32_t *)(0x40010C00))

#define GPIOA_CRL			((uint32_t *)(GPIO_A_BASE + 0x00))
#define GPIOA_CRH			((uint32_t *)(GPIO_A_BASE + 0x04))
#define GPIOA_IDR			((uint32_t *)(GPIO_A_BASE + 0x08))
#define GPIOA_ODR			((uint32_t *)(GPIO_A_BASE + 0x0C))
#define GPIOA_BSRR			((uint32_t *)(GPIO_A_BASE + 0x10))
#define GPIOA_BRR			((uint32_t *)(GPIO_A_BASE + 0x14))
#define GPIOA_LCKR			((uint32_t *)(GPIO_A_BASE + 0x18))

#define GPIOB_CRL			((uint32_t *)(GPIO_B_BASE + 0x00))
#define GPIOB_CRH			((uint32_t *)(GPIO_B_BASE + 0x04))
#define GPIOB_IDR			((uint32_t *)(GPIO_B_BASE + 0x08))
#define GPIOB_ODR			((uint32_t *)(GPIO_B_BASE + 0x0C))
#define GPIOB_BSRR			((uint32_t *)(GPIO_B_BASE + 0x10))
#define GPIOB_BRR			((uint32_t *)(GPIO_B_BASE + 0x14))
#define GPIOB_LCKR			((uint32_t *)(GPIO_B_BASE + 0x18))

#define RCC_CR				((uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR			((uint32_t *)(RCC_BASE + 0x04))
#define RCC_CIR				((uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB2RSTR		((uint32_t *)(RCC_BASE + 0x0C))
#define RCC_APB1RSTR		((uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHBENR			((uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR         ((uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR         ((uint32_t *)(RCC_BASE + 0x1C))
#define RCC_BDCR			((uint32_t *)(RCC_BASE + 0x20))
#define RCC_CSR				((uint32_t *)(RCC_BASE + 0x24))
#define RCC_AHPRSTR			((uint32_t *)(RCC_BASE + 0x28))
#define RCC_CFGR2			((uint32_t *)(RCC_BASE + 0x2C))



typedef struct
{
	volatile uint32_t *CR;
	volatile uint32_t *CFGR;
	volatile uint32_t *CIR;
	volatile uint32_t *APB2RSTR;
	volatile uint32_t *APB1RSTR;
	volatile uint32_t *AHBENR;
	volatile uint32_t *APB2ENR;
	volatile uint32_t *APB1ENR;
	volatile uint32_t *BDCR;
	volatile uint32_t *CSR;
	volatile uint32_t *AHPRSTR;
	volatile uint32_t *CFGR2;
}RCC_struct;

RCC_struct RCC_ADDR = {	RCC_CR,RCC_CFGR,RCC_CIR,RCC_APB2RSTR,RCC_APB1RSTR,
						RCC_AHBENR,RCC_APB2ENR,RCC_APB1ENR,RCC_BDCR,RCC_CSR,
						RCC_AHPRSTR,RCC_CFGR2};

RCC_struct * const RCC = &RCC_ADDR;

typedef struct
{
	volatile uint32_t *CRL;
	volatile uint32_t *CRH;
	volatile uint32_t *IDR;
	volatile uint32_t *ODR;
	volatile uint32_t *BSRR;
	volatile uint32_t *BRR;
	volatile uint32_t *LCKR;
}GPIOA_struct;
GPIOA_struct GPIOA_ADDR = {	GPIOA_CRL,GPIOA_CRH,GPIOA_IDR,GPIOA_ODR,
							GPIOA_BSRR,GPIOA_BRR,GPIOA_LCKR};
GPIOA_struct * const GPIOA =  &GPIOA_ADDR;

typedef struct
{
	volatile uint32_t *CRL;
	volatile uint32_t *CRH;
	volatile uint32_t *IDR;
	volatile uint32_t *ODR;
	volatile uint32_t *BSRR;
	volatile uint32_t *BRR;
	volatile uint32_t *LCKR;
}GPIOB_struct;

GPIOB_struct GPIOB_ADDR = {	GPIOB_CRL,GPIOB_CRH,GPIOB_IDR,GPIOB_ODR,
							GPIOB_BSRR,GPIOB_BRR,GPIOB_LCKR};
GPIOB_struct * const GPIOB = &GPIOB_ADDR;

#endif /* REGISTER_H_ */
