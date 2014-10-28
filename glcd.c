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
	GLCD_Write(3,0,0x3F); 		        //GLCD bekapcsol�sa
	GLCD_Write(3,0,0xC0);			//GLCD Start Line
	GLCD_Clear();				//Kijelz� t�rl�se
}

//******************************************************************************************
// @le�r�s: Kijelz� meghajt� kimenet�t enged�lyez� f�ggv�ny
// @param�ter: newState = ENABLE, #GLCDEN = 0
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
// @le�r�s: Be�rja a g_data �rt�ket a kiv�lasztott kijelz�vez�rl�be (cs_s->CS1, CS2)
// 			utas�t�s/adat param�ternek megfelel�en.
// 			�ltal�nosan felhaszn�lhat� 8bit (adat/utas�t�s) be�r�s�ra a kijelz� vez�rl�j�be.
// @param�ter: cs_s, 1 = CS1, 2 = CS2, 3 = CS1&CS2
// @param�ter: d_i, 0 = instruction, 1 = data
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
// @le�r�s: K�zvetlen�l t�rli a kijelz�t.
// @param�ter: nincs
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
// @le�r�s: A kijelz� adott sor-oszlop metszetet �ll�tja az m_data �rt�knek megfelel�en.
// @param�ter: m_data: adott metszet rajzolata hex�ba k�dolva (l�sd.: BitFont.excel
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
// @le�r�s: Kijelz� �r�s-k�sleltet�si id�.
//******************************************************************************************
void GLCD_Delay(char value)
{
	Sys_DelayUs(value);
}
