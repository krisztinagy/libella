#include <stddef.h>
#include "stm32f4xx.h"
#include "configurations.h"
#include "globalfunctions.h"	// Sys_Delay
#include "LIS3DSH.h"
#include "glcd.h"

// Prototypes
void System_Configuration(void);
void GPIO_Configuration(void);
void TIMER3_Configuration(int pwm_cntr);
void I2C_Configuration(void);

//Defining variables
GPIO_InitTypeDef  GPIO_InitStructure;
TIM_TimeBaseInitTypeDef  TIMx_TimeBaseStructure;
TIM_OCInitTypeDef  TIMx_OCInitStructure;
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;


//Initializing function: calls all init functions
void System_Configuration(void)
{
	SysTick_Config(SystemCoreClock/1000000);
	GPIO_Configuration();
	TIMER3_Configuration(1000);
	// Grafikus LCD inicializalasa
	GLCD_Init();
	// SPI (MEMS) konfiguralasa
        init_SPI1();
}

//GPIO Config
void GPIO_Configuration()
{
/*
 ===============================================================================
                      GPIO - clk enable
 ===============================================================================
*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
/*
 ===============================================================================
                      Definig IN and OUTPUTS
 ===============================================================================
*/

	// GPIOB - #GLCDEN, GLCD_CS1, GLCD_CS2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	// GPIOC - PUSHBUTTON1, PUSHBUTTON2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
        

	// GPIOD - GLCD_RESET, GLCD_DI, GLCD_E, DISCOVERY-LEDS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	// GPIOE - DB0-DB7, RW lábak beállítása
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	// Eszközök letiltása - alap állapot bekapcsolása
	GPIO_ResetBits(GPIOE,GPIO_Pin_7);	//GLCD_RW = 0
	GPIO_SetBits(GPIOD, GPIO_Pin_7); 	//GLCD_E = 1
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//GLCD_RESET = 0
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);	//#GLCDEN = 0

/*
 ===============================================================================
                      Alternate functions 
 ===============================================================================
*/

	/* ++++++++++++++++++++++ TIMER Config ++++++++++++++++++++++ */

	// Timer3_CH2 - PC7, GLCD BackLight PWM
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


//CONFIGURING LCD BACKLIGHT
void TIMER3_Configuration(int pwm_cntr)
{
	
	 //* APB1 = 42MHz
	 //* fpwm = TIM3_CLK/(Period+1)
	 //* TIM3_CLK = APB1_CLK / (prescaler + 1)
	 //* Duty_Cycle = (TIM_Pulse / Tim_Period)*100
	
  
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIMx_TimeBaseStructure.TIM_Prescaler = 349;
	TIMx_TimeBaseStructure.TIM_Period = 2399;
	TIMx_TimeBaseStructure.TIM_ClockDivision = 0;
	TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIMx_TimeBaseStructure);

	TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIMx_OCInitStructure.TIM_Pulse = pwm_cntr;	
	TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC2Init(TIM3, &TIMx_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
        
        
        NVIC_InitTypeDef NVIC_InitStructure;
        
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
 
        
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
        NVIC_Init(&NVIC_InitStructure); 

        
        
        TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}

