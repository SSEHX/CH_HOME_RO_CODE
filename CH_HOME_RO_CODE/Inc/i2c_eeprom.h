#ifndef __I2C_EEPROM_H__
#define        __I2C_EEPROM_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
 
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS       0x0A
 
#define I2C_WR                0                /* д����bit */
#define I2C_RD                1                /* ������bit */
 
#define I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_GPIO_PORT                       GPIOB   
#define I2C_SCL_PIN                         GPIO_PIN_10
#define I2C_SDA_PIN                         GPIO_PIN_11
 
#define I2C_SCL_HIGH()                      HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define I2C_SCL_LOW()                       HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define I2C_SDA_HIGH()                      HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define I2C_SDA_LOW()                       HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define I2C_SDA_READ()                      HAL_GPIO_ReadPin(I2C_GPIO_PORT,I2C_SDA_PIN)
 
/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
 
/* AT24C01/02ÿҳ��8���ֽ� 
 * AT24C04/08A/16Aÿҳ��16���ֽ� 
 */
#define EEPROM_DEV_ADDR                       0xA0                    /* 24xx02���豸��ַ */
#define EEPROM_PAGE_SIZE                      16                      /* 24xx02��ҳ���С */
#define EEPROM_SIZE                           256                     /* 24xx02������ */
 
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void    I2C_Start(void);
void    I2C_Stop(void);
void    I2C_SendByte(uint8_t _ucByte);
uint8_t I2C_ReadByte(void);
uint8_t I2C_WaitAck(void);
void    I2C_Ack(void);
void    I2C_NAck(void);
uint8_t I2C_CheckDevice(uint8_t _Address);
 
uint8_t EEPROM_CheckOk(void);
uint8_t EEPROM_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t EEPROM_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
 
 
 
#endif /* __I2C_EEPROM_H__ */
 
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/