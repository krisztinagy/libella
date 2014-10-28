#include "stm32f4_discovery.h"
#include "main.h"

#include "configurations.h"
#include "globalfunctions.h"
#include "glcd.h"
#include "ZonaTerkep.h"
#include "LIS3DSH.h"

#include <stdio.h>

//----------------------------------------------------------------------------------------------------------------
//-------------------------Declaring and setting variables--------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------


static __IO uint32_t TimingDelay;
uint8_t forceStart = 0;

//MEMS sensor data is written in these variables
//due to noise and LCD resolution only the higher bits are important
int8_t high_x = 0;
int8_t high_y = 0;

//the distance from the middle of LCD
int8_t radius_x =0;
int8_t radius_y =0;

//signed!
int16_t value = 0;

//x and y coordinates of the cirle's centre
uint8_t pixel_x=0;
uint8_t pixel_y=0;
//X coordinate's "cordinates" in LCD's hex codes
char column=0;
char c_bit_aux=0;
char c_bit=0;


//the moving bubble
//coordinates of the pixels from the centre of the bubble
int8_t pixels_x[] = {8, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7, -8, -8, -9, -9,
                    -9, -9, -8, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8};
int8_t pixels_y[] = {0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 8, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0,
                    -1, -2, -3, -4, -5, -6, -7, -8, -8, -9, -9, -9, -9, -8, -8, -7, -6, -5, -4, -3, -2, -1};
int8_t pixels_x_aux[] ={8, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7, -8, -8, -9, -9
                    -9, -9, -8, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8};
int8_t pixels_y_aux[] = {0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 8, 8, 7, 7, 6, 5, 4, 3, 2, 1, 0,
                    -1, -2, -3, -4, -5, -6, -7, -8, -8, -9, -9, -9, -9, -8, -8, -7, -6, -5, -4, -3, -2, -1};

//auxiliary variables for creating the actual pixels
int8_t power =0;
char power_result=1;

//actual_hex_data[] is the block in which the actual bubble position is written in hex data, making the controlling of the LCD simple
//actual_hex_data_index points to the element to be written in this block
int16_t actual_hex_data_index =0;
char actual_hex_data[1024];

//IRQ handling PUSHBUTTONS: previous and current values
volatile uint8_t pb_plus_prev = 1; 
volatile uint8_t pb_minus_prev = 1;
volatile uint8_t pb_plus_curr = 1; 
volatile uint8_t pb_minus_curr = 1;

//LCD baclight PWM counter
volatile int pwm_cntr=5;

//----------------------------------------------------------------------------------------------------------------
//-------------------------Setting backlight of LCD---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------


void TIM3_IRQHandler(void)
{
   //checking the reason of the interrupt
   if ( TIM_GetITStatus(TIM3, TIM_IT_Update)) 
   { 
     //Reading Pin values
     pb_plus_curr = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
     pb_minus_curr = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9);
     
     //Searching for falling edges
     if( !pb_plus_curr && pb_plus_prev)
     {
       //If minus button pushed, decrement the backlight
       if(pwm_cntr < 10)
       {
         pwm_cntr++;
       }
       
     }
     //Searching for falling edges
     if(!pb_minus_curr && pb_minus_prev)
     {
       //If plus button pushed, increment the backlight
       if(pwm_cntr > 1)
       {
         pwm_cntr--;
       }
       
     }
    
     //Storing the actual value in the previous value
     pb_minus_prev = pb_minus_curr;
     pb_plus_prev = pb_plus_curr;
     
     //Resetting LCD Backlight
     TIM_OCInitTypeDef  TIMx_OCInitStructure;
     
     TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
     TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
     TIMx_OCInitStructure.TIM_Pulse = pwm_cntr*200;	// ((pwm_cntr*100/2399)*100)%
     TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OC2Init(TIM3, &TIMx_OCInitStructure);
     TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
     
     //Clearing IT flag
     TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
   }

  
}


int main(void)
{    
        //declaring iterating variables
        int i=0;
        int j=0;;
        int k=0;
        
        //Calling configuring functions
	System_Configuration();
        lis3dsh_config();
	GLCD_Clear();
        
	//Infinite cycle
	while(1){
          
          //------------------------------------------------------------------------------------
          //-------------------------Creating moving bubble-------------------------------------
          //------------------------------------------------------------------------------------
		 
		//X axis high (because of noise, only high bits are important)
		GPIO_ResetBits(GPIOE, GPIO_Pin_3);
		SPI1_send(0x29 | 0x80);
		high_x = SPI1_send(0x00);
		GPIO_SetBits(GPIOE, GPIO_Pin_3);
                
                //Y axis high (because of noise, only high bits are important)
                GPIO_ResetBits(GPIOE, GPIO_Pin_3);
		SPI1_send(0x2B | 0x80);
		high_y = SPI1_send(0x00);
		GPIO_SetBits(GPIOE, GPIO_Pin_3);


                Sys_DelayMs(100);
                

                //X pixel
                pixel_x = (int8_t)( 64-((high_x / 2 / 1.45)+32));
                radius_x = pixel_x - 32;
                
                c_bit_aux = pixel_x % 8;
                c_bit = 0x01<<c_bit_aux;
                
                column= pixel_x / 8;
                
                //Y pixel
                pixel_y = (int8_t)(((high_y / 2 / 1.45)+64));
                radius_y = pixel_y - 64;
                
                //creating pixels to be set
                for(i=0; i<44; i++)
                {
                    pixels_x_aux[i] = pixels_x[i] + pixel_x;
                    pixels_y_aux[i] = pixels_y[i] + pixel_y;
                }
                
                //clearing previous pixels
                for(i=0; i<1024; i++)
                {
                  actual_hex_data[i]=0x00;
                }
                
                //setting new pixels
                for(i=0; i<44; i++)
                {
                  power_result=1;
                  actual_hex_data_index= ((int)(pixels_x_aux[i] / 8)) * 128 + pixels_y_aux[i];
                  power=(pixels_x_aux[i] - ((int) (pixels_x_aux[i] / 8)) * 8);
                  for(j=0; j<power; j++)
                  {
                    power_result*=2;
                  }
                  actual_hex_data[actual_hex_data_index] += power_result;
                }
                
                //drawing the union of ZonaTerkep and actual_hex_data
                j=0;
                k=0;
                for (i=0;i<8;i++)
                {
                    for(j=0;j<128;j++)
                    {
                            GLCD_Write_Block(ZonaTerkep[k] | actual_hex_data[k] ,i,j);
                            k++;
                    }
                }   

          
	}
}


