/**
*****************************************************************************
** Kommunikációs mérés - configurations.h
** Rendszer konfiguráció, inicializáció header file-ja
*****************************************************************************
*/

#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_

#define TempAddr ((uint8_t) 0x30)
#define TempConfReg ((uint8_t) 0x01)
#define TempReg ((uint8_t) 0x05)
#define I2C_TIMEOUT ((uint16_t) 0xFFFF)

void System_Configuration(void);
void SW_ReConfig(void);

#endif /* CONFIGURATIONS_H_ */
