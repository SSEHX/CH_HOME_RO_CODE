#ifndef __APP_H__
#define	__APP_H__
#include "string.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "bc95.h"
#include "i2c_eeprom.h"

#define QR_CODE_LEN 20

#define ADC_CHANNEL_NUMBER      7
#define ADC_LOOP_NUMBER         5

typedef struct {
    uint8_t     booting;                // 已经启动
    uint8_t     processing_status;
    uint8_t     bc95_run;               // bc95正常运行
    uint32_t    boot;                   // 设备开关机
    uint32_t    arrears_boot;           // 停机开关机
    uint32_t    device_registe;         // 设备注册状态
    uint32_t    device_mode;            // 设备类型
    uint16_t    pure_tds;               // 纯水TDS 
    uint16_t    raw_tds;                // 原水TDS
    uint32_t    time;                   // 租赁时间
    uint32_t    flow;                   // 流量
    uint32_t    rinse;                  // 冲洗
    uint8_t     rinse_time;             // 冲洗时间
    uint32_t    filter[10];             // 滤芯
    uint32_t    create_water_time_m;    // 制水时间分钟
    uint8_t     flow_or_time;           // 租赁是用流量还是时间
    uint8_t     create_water_time_rinse;// 制水多长时间冲洗 刻钟 15分钟一刻钟
    uint32_t    proportion;             // 时间换算比例
    uint8_t     use_water_table;        // 是否使用水位
    uint8_t     qr_code[QR_CODE_LEN+1]; // 二维码
}device_statusTypeDef;

extern device_statusTypeDef device_status; 

typedef struct{
    uint8_t     bc95_init_error;
    uint8_t     leakage;                    // leakage flag
    uint8_t     leakage_sended;             // 漏水错误已发送
    uint8_t     raw_no_water;               // no water flag
    uint8_t     raw_no_water_sended;        // 缺水错误已发送
    uint8_t     create_water_too_long;
    
}device_errorTypeDef;

extern device_errorTypeDef device_error;

extern uint32_t device_adc_value[ADC_CHANNEL_NUMBER*ADC_LOOP_NUMBER-1];

void boot_rinse();
uint8_t registe_device();

#endif