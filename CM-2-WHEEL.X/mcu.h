#ifndef MCU_H_
#define MCU_H_

//#define MCU_CRYSTAL

//#define PWM1_SET(duty) CCPR1L = (duty)
//#define PWM3_SET(duty) CCPR3L = (duty)

//spi controlado por software
#define SPI_CLOCK LATCbits.LATC3
#define SPI_DO LATCbits.LATC5
#define SPI_DI PORTCbits.RC4

#define TMR1_OVERFLOW() PIR1bits.TMR1IF

//#define FLASH_ERASE_BLOCK 64

void MCU_Init(void);
void MCU_EnableInterrupts(void);
void MCU_DisableInterrupts(void);

void PWM1_Init(void);
void PWM1_ConfirmDutyCycle(void);
void PWM1_Close(void);
void PWM3_Init(void);
void PWM3_Write(UInt16 pwm);
void PWM4_Init(void);
void PWM4_Write(UInt16 pwm);

void SPI_Init(void);
UInt8 SPI_WriteReadBus(UInt8 dataOut);

void TMR0_Init(void);
void TMR1_Init(void);
void TMR1_Start(void);
void TMR1_Stop(void);

void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void INT3_Init(void);

void EEPROM_WriteByte(UInt16 address, UInt8 value);
UInt8 EEPROM_ReadByte(UInt16 address);
void EEPROM_Refresh(void);
UInt8 EEPROM_ValidWriteByte(UInt16 address, UInt8 value);

//UInt16 Flash_Read(UInt24 address);
//void Flash_Erase(UInt24 startAddr, UInt24 endAddr);
//void Flash_WriteUInt16(UInt24 startAddr, UInt16 data);

#endif /* MCU_H_ */
