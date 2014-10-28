#include "stm32f4xx.h"

int sys_delay = 0;

//******************************************************************************************
// @leírás: SysTick megszakító rutin csökkenti a sys_delay értékét.
// @paraméter: ms: várakozási idõ us-ban.
//******************************************************************************************
void Sys_DelayUs(int us){
	sys_delay = us;
	while(sys_delay);
}

void Sys_DelayMs(int ms){
	sys_delay = ms*1000;
	while(sys_delay);
}

