#include "stm32f4xx.h"

int sys_delay = 0;

//******************************************************************************************
// @le�r�s: SysTick megszak�t� rutin cs�kkenti a sys_delay �rt�k�t.
// @param�ter: ms: v�rakoz�si id� us-ban.
//******************************************************************************************
void Sys_DelayUs(int us){
	sys_delay = us;
	while(sys_delay);
}

void Sys_DelayMs(int ms){
	sys_delay = ms*1000;
	while(sys_delay);
}

