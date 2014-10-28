#ifndef MAIN_H
#define MAIN_H

#define LEDport 	GPIOD
#define LEDy 		GPIO_Pin_0
#define LEDg 		GPIO_Pin_1
#define LEDr 		GPIO_Pin_2

#define TEMPport	GPIOA
#define TEMPss 		GPIO_Pin_4

#define MEMaddr		0xA0

const u16 LEDS = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;


/*** INIT & SYSTEM *****************************************************************/
void initLeds(void);
void initSPI(void);
void initI2C(void);
void d_us(u32 us);
void d_ms(u32 ms);

/*** SPI ***************************************************************************/
uint16_t SPI_rec(void);
int16_t getTemp(void);

/*** I2C ***************************************************************************/
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
void I2C_stop(I2C_TypeDef* I2Cx);
void wrMem(uint16_t addr, uint8_t data);
uint8_t rdMem(uint16_t addr);


#endif