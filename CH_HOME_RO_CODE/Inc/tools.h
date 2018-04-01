#ifndef __TOOLS_H__
#define	__TOOLS_H__
#include "main.h"
#include "stm32f1xx_hal.h"
#include "app.h"

#define FLASH_DATA_ADDR 0x0800F000
#define FLASH_DATA_LEN  50

#define FIRST_BOOT_ADDR     0
#define REGISTE_ADDR        1                                         //
#define CREATE_WATER_TIME_M 2
#define CREATE_WATER_FIRST  3
#define TIME_ADDR           4                                         //
#define FLOW_ADDR           5                                         //
#define BOOT_ADDR           6                                         //
#define DEVICE_MODE_ADDR    7         //1:租赁  2：零售               //  

#define FILTER_ADDR         8         //10个滤芯，依次后排    到19    //

#define RINSE_TIME          19        //冲洗时间                      //
#define ARREARS_BOOT_ADDR   20        //停机开关机                    //   
#define QR_CODE_ADDR        21        //二维码                        //


extern uint32_t adc_value[60];

#define LOW_SWITCH_PORT         GPIOC
#define LOW_SWITCH_PIN          GPIO_PIN_11
#define HIGH_SWITCH_PORT        GPIOC
#define HIGH_SWITCH_PIN         GPIO_PIN_10

#define RINSE_PORT              GPIOB
#define RINSE_PIN               GPIO_PIN_4

#define CREATE_WATER_PORT       GPIOB
#define CREATE_WATER_PIN        GPIO_PIN_3

#define PUMP_PORT               GPIOC
#define PUMP_PIN                GPIO_PIN_12

#define SYSTEM_LED_PORT         GPIOA
#define SYSTEM_LED_PIN          GPIO_PIN_12
#define _system_led             HAL_GPIO_TogglePin(SYSTEM_LED_PORT, SYSTEM_LED_PIN)

#define NETWORK_LED_PORT        GPIOB
#define NETWORK_LED_PIN         GPIO_PIN_9
#define _network_led            HAL_GPIO_TogglePin(NETWORK_LED_PORT, NETWORK_LED_PIN)

extern uint32_t flash_pages_data[FLASH_DATA_LEN];

uint8_t *ByteToHexStr(const uint8_t* source, uint32_t sourceLen);

void start_adc();

void flash_write(uint32_t *data);
void flash_write_word(uint8_t address, uint32_t data);
uint32_t flash_read(uint32_t addr);

void processing_server_command();
void flash_device_status();

uint8_t get_low_switch();
uint8_t get_high_switch();

void open_create_water();
void shut_create_water();
void open_wdi();
void close_wdi();
void feed_wdi();
void start_create_water(); 
void open_rinse();
void stop_create_water();  
void shut_rinse();
void start_rinse();
void stop_rinse();

void status_error_led(GPIO_PinState status);
void status_network_led(GPIO_PinState status);

void beep(uint8_t ms);
#endif