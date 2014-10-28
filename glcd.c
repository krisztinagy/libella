#include "stm32f4xx.h"
#include "glcd.h"
#include "globalfunctions.h"



//******************************************************************************************
// Initializing screen 
// Timing is based on GLCD_delay
//******************************************************************************************
void GLCD_Init(void)
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);	//GLCD_E = 0
	GPIO_SetBits(GPIOD, GPIO_Pin_3);	//GLCD_RESET = 1
	GLCD_Write(3,0,0x3F); 		        //GLCD bekapcsolása
	GLCD_Write(3,0,0xC0);			//GLCD Start Line
	GLCD_Clear();				//Kijelzõ törlése
}

//******************************************************************************************
// @leírás: Kijelzõ meghajtó kimenetét engedélyezõ függvény
// @paraméter: newState = ENABLE, #GLCDEN = 0
//			   newStare = DISABLE, #GLCDEN = 1
//******************************************************************************************
void GLCDEN(FunctionalState newState)
{
	if(newState)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_7);
	}
}

//******************************************************************************************
// @leírás: Beírja a g_data értéket a kiválasztott kijelzõvezérlõbe (cs_s->CS1, CS2)
// 			utasítás/adat paraméternek megfelelõen.
// 			Általánosan felhasználható 8bit (adat/utasítás) beírására a kijelzõ vezérlõjébe.
// @paraméter: cs_s, 1 = CS1, 2 = CS2, 3 = CS1&CS2
// @paraméter: d_i, 0 = instruction, 1 = data
//******************************************************************************************
void GLCD_Write(char cs_s,char d_i,char g_data)
{
	uint16_t data = 0x0000;
	
	switch(cs_s){
		case 1:
			GPIO_SetBits(GPIOB, GPIO_Pin_4);	//CS1 = 1
			break;
		case 2:
		 	GPIO_SetBits(GPIOB, GPIO_Pin_5);	//CS2 = 1
			break;
		case 3:
			GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5);	//CS1 = 1, CS2 = 1
			break;
	}
	
	switch(d_i)
	{
		case 0:
			GPIO_ResetBits(GPIOD, GPIO_Pin_6);	//PD6 = 0 -> Instruction
			break;
		case 1:
			GPIO_SetBits(GPIOD, GPIO_Pin_6);	//PD6 = 1 -> Data
			break;
	}

	data = GPIOE->IDR;
	data &= 0x00FF;
	data |= g_data << 8;
	GPIOE->ODR = data;

	GLCD_Delay(1);
	GPIO_SetBits(GPIOD,GPIO_Pin_7);		//GLCD_E = 1
	GLCD_Delay(2);
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);	//GLCD_E = 0
	GLCD_Delay(4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);	//CS1 = 0
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//CS2 = 0
}
//******************************************************************************************
// @leírás: Közvetlenül törli a kijelzõt.
// @paraméter: nincs
//******************************************************************************************
void GLCD_Clear(void)
{
	char x,y;
	for(x=0;x<8;x++)
	{
		GLCD_Write(3,0,0x40);
		GLCD_Write(3,0,(0xB8|x));
		for(y=0;y<64;y++)
		{
			GLCD_Write(3,1,0x00);
	  	}
	}
}
//******************************************************************************************
// @leírás: A kijelzõ adott sor-oszlop metszetet állítja az m_data értéknek megfelelõen.
// @paraméter: m_data: adott metszet rajzolata hexába kódolva (lásd.: BitFont.excel
//			   cX: sor (0-7)
//			   cY: oszlop (0-127)
//******************************************************************************************
void GLCD_Write_Block(char m_data,char cX,char cY)
{
	char chip=1;
	if(cY>=64){
	chip=2;
	cY-=64;
	}
	GLCD_Write(chip,0,(0x40|cY));
	GLCD_Write(chip,0,(0xB8|cX));
	GLCD_Write(chip,1,m_data);
}

//******************************************************************************************
// @leírás: Kijelzõ írás-késleltetési idõ.
//******************************************************************************************
void GLCD_Delay(char value)
{
	Sys_DelayUs(value);
}
