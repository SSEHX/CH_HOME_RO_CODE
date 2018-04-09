#include "tools.h"
#include <stdlib.h>

uint32_t adc_value[30] = {0};
uint32_t flow_count = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM3){
        flow_count++;
        if(flow_count >= 740){
            flow_count = 0;
            device_status.flow++;
        }
    }
}

void start_create_water_timer(){
    HAL_TIM_Base_Start_IT(&htim1);
}

void stop_create_water_timer(){
    HAL_TIM_Base_Stop_IT(&htim1);
}

void word_2_eeprom(uint8_t *eeprom_data, uint32_t data){
    eeprom_data[0] = (data >> 24) & 0x000000ff;
    eeprom_data[1] = (data >> 16) & 0x000000ff;
    eeprom_data[2] = (data >> 8)  & 0x000000ff;
    eeprom_data[3] = data & 0x000000ff;
}

uint32_t eeprom_2_word(uint8_t *eeprom_data){
    uint32_t temp = 0;
    temp |= eeprom_data[0] << 24;
    temp |= eeprom_data[1] << 16;
    temp |= eeprom_data[2] << 8;
    temp |= eeprom_data[3];
    
    return temp;
}

/*
+-------- 0 --------+-------- 1 --------+-------- 18 --------+
|     首次开机      |     停机状态      |  使用流量还是时间  |
+-------------------+-------------------+--------------------+

+------- 19 --------+------- 20 --------+
|     冲洗时间      |  制水多少时间冲洗 |
+-------------------+-------------------+

+-------- 2 - 3 - 4 - 5 --------+-------- 6 - 7 - 8 - 9 --------+
|          已使用流量           |      服务器下发剩余流量       |
+-------------------------------+-------------------------------+

+------ 10 - 11 - 12 - 13 ------+------ 14 - 15 - 16 - 17 ------+
|          已制水时间           |      服务器下发剩余时间       |
*-------------------------------+-------------------------------+

+--- 21 -- 22 -- 23 -- 24 -- 25 -- 26 -- 27 -- 28 -- 29 -- 30---+
|     1     2     3     4     5     6     7     8     9    10   |   滤芯
*---------------------------------------------------------------+

*/

void eeprom_save_device_status(uint8_t set_zero){
    uint8_t eeprom_temp[31] = {0};
    //uint8_t j = 0;
    
    if(!set_zero){
        eeprom_temp[0]  = device_status.device_registe;             //首次开机
        eeprom_temp[1]  = device_status.arrears_boot;               //停机开关机
        eeprom_temp[18] = device_status.flow_or_time;               //使用流量还是时间
        eeprom_temp[19] = device_status.rinse_time;                 //冲洗时间
        eeprom_temp[20] = device_status.create_water_time_rinse;    //制水多长时间冲洗
        
        word_2_eeprom(&eeprom_temp[2], device_status.flow);
        word_2_eeprom(&eeprom_temp[6], device_status.server_flow);
        word_2_eeprom(&eeprom_temp[10], device_status.create_water_time_m);
        word_2_eeprom(&eeprom_temp[14], device_status.server_time);
        
        //滤芯剩余
        for(uint8_t i = 0 ; i < 10 ; i++){
            eeprom_temp[21+i] = device_status.filter[i];
        }
    }

    if(EEPROM_CheckOk()){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        EEPROM_WriteBytes(eeprom_temp, 1, sizeof(eeprom_temp));
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }
}

void eeprom_read_device_status(){
    uint8_t eeprom_temp[31] = {0};
    
    if(EEPROM_CheckOk()){
        EEPROM_ReadBytes(eeprom_temp, 1, sizeof(eeprom_temp));
    }
    
    device_status.device_registe = eeprom_temp[0];               //首次开机
    device_status.arrears_boot =  eeprom_temp[1];                //停机开关机
    device_status.flow_or_time = eeprom_temp[18];                //使用流量还是时间
    device_status.rinse_time = eeprom_temp[19];                  //冲洗时间
    device_status.create_water_time_rinse = eeprom_temp[20];     //制水多长时间冲洗
    
    //滤芯剩余
    for(uint8_t i = 0 ; i < 10 ; i++){
        device_status.filter[i] = eeprom_temp[21+i];
    }
    
    device_status.flow = eeprom_2_word(&eeprom_temp[2]);
    device_status.server_flow = eeprom_2_word(&eeprom_temp[6]);
    device_status.create_water_time_m = eeprom_2_word(&eeprom_temp[10]);
    device_status.server_time = eeprom_2_word(&eeprom_temp[14]);
}

void start_adc(){
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_value, 30);
}

void stop_adc(){
    HAL_ADC_Stop_DMA(&hadc1);
    memset(&adc_value, 0 ,sizeof(adc_value));
}

uint8_t get_low_switch(){
    return HAL_GPIO_ReadPin(LOW_SWITCH_PORT, LOW_SWITCH_PIN);
}

uint8_t get_high_switch(){
    return HAL_GPIO_ReadPin(HIGH_SWITCH_PORT, HIGH_SWITCH_PIN);
}

uint8_t *ByteToHexStr(const uint8_t* source, uint32_t sourceLen)  
{  
    uint8_t a[100] = {0};
    uint8_t *dest = a;
    short i;  
    unsigned char highByte, lowByte;  
    
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;
        
        highByte += 0x30;  
        
        if (highByte > 0x39)  
            dest[i * 2] = highByte + 0x07;  
        else  
            dest[i * 2] = highByte;  
        
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }
    return dest;  
}

void processing_server_command(){
    //    0  2  4  6        14       22 24 26 28 30 32 34 36 38 40 42 44 46
    //    01 01 00 00000000 00000000 00 64 64 64 64 64 64 64 64 64 64 01 43482D524F2D3030303031232323232323232323
    /*!< A 设备开机 */
    uint8_t boot[3] = {0};              
    memmove(boot, &bc95_recv.server_cmd[0], 2);
    device_status.boot = atoi((const char*)boot);
    
    /*!< B 停机开关机 */
    uint8_t arrears_boot[3] = {0};
    memmove(arrears_boot, &bc95_recv.server_cmd[2], 2);
    sscanf((const char*)arrears_boot, "%x", &device_status.arrears_boot);
    
    /*!< C 冲洗开关 */
    uint8_t rinse[3] = {0};
    memmove(rinse, &bc95_recv.server_cmd[4], 2);
    sscanf((const char*)rinse, "%x", &device_status.rinse);
    
    /*!< D 时间 */
    uint8_t server_time[9] = {0};
    memmove(server_time, &bc95_recv.server_cmd[6], 8);
    sscanf((const char*)server_time, "%x", &device_status.server_time);
    
    /*!< F 流量 */
    uint8_t server_flow[9] = {0};
    memmove(server_flow, &bc95_recv.server_cmd[14], 8);
    sscanf((const char*)server_flow, "%x", &device_status.server_flow);
    
    /*!< F 设备类型  */
    uint8_t device_mode[3] = {0};
    memmove(device_mode, &bc95_recv.server_cmd[22], 2);
    sscanf((const char*)device_mode, "%x", &device_status.device_mode);
    
    /*!< g1-g10 滤芯 */
    for(uint8_t i = 0 ; i < 10 ; i++){
        uint8_t filter[3] = {0};
        memmove(filter, &bc95_recv.server_cmd[24 + (i * 2)], 2);
        sscanf((const char*)filter, "%02x", &device_status.filter[i]);
    }
    
    /*!< H 注册是否成功*/
    uint8_t device_registe[3] = {0};
    memmove(device_registe, &bc95_recv.server_cmd[44], 2);
    sscanf((const char*)device_registe, "%02x", &device_status.device_registe);
    
    /*!< I 二维码 */
    uint8_t qr_code[(QR_CODE_LEN*2)+1] = {0};
    memmove(qr_code, &bc95_recv.server_cmd[46], (sizeof(device_status.qr_code) * 2));
    for(uint8_t i = 0 ; i < QR_CODE_LEN ; i++){
        uint8_t temp[3] = {0};
        memmove(temp, &qr_code[ (i*2) ], 2);
        sscanf((const char*)temp , "%02x", (uint32_t *)&device_status.qr_code[i]);
    }
    
    /*!< I 冲洗时间 */
    uint8_t rinse_time[3] = {0};
    memmove(rinse_time, &bc95_recv.server_cmd[86], 2);
    sscanf((const char*)rinse_time, "%02x", &device_status.rinse_time);
    //0101000000003C000F424001646464646464646464640143482D4E422D3036343639232323232323232323 
    //06 00 02 00000258 00
    
    // 制水多长时间冲洗
    uint8_t create_water_time_rinse[3] = {0};
    memmove(create_water_time_rinse, &bc95_recv.server_cmd[88], 2);
    sscanf((const char*)create_water_time_rinse, "%02x", &device_status.create_water_time_rinse);
    
    // 流量还是时间
    uint8_t flow_or_time[3] = {0};
    memmove(flow_or_time, &bc95_recv.server_cmd[90], 2);
    sscanf((const char*)flow_or_time, "%02x", &device_status.flow_or_time);
    
    /*!< F 换算比例 */
    uint8_t proportion[9] = {0};
    memmove(proportion, &bc95_recv.server_cmd[92], 8);
    sscanf((const char*)proportion, "%x", &device_status.proportion);
    
    // 用不用水位
    uint8_t use_water_table[3] = {0};
    memmove(use_water_table, &bc95_recv.server_cmd[100], 2);
    sscanf((const char*)use_water_table, "%02x", &device_status.use_water_table);
    
    //保存所有设备状态到eeprom
    eeprom_save_device_status(NO);
    
    //设置处理状态为成功 -> 发送当前状态 -> 重置处理状态
    device_status.processing_status = 1;
    bc95_send_coap(cmd_coap_send_ok);
    device_status.processing_status = 0;
    
}

void open_create_water(){
    HAL_GPIO_WritePin(CREATE_WATER_PORT, CREATE_WATER_PIN, GPIO_PIN_RESET);
}

void shut_create_water(){
    HAL_GPIO_WritePin(CREATE_WATER_PORT, CREATE_WATER_PIN, GPIO_PIN_SET);
}

void open_rinse(){
    HAL_GPIO_WritePin(RINSE_PORT, RINSE_PIN, GPIO_PIN_RESET);
    
}

void shut_rinse(){
    HAL_GPIO_WritePin(RINSE_PORT, RINSE_PIN, GPIO_PIN_SET);
}

void open_pump(){
    HAL_GPIO_WritePin(PUMP_PORT, PUMP_PIN, GPIO_PIN_RESET);
    start_create_water_timer();         //开启计费定时器
}

void shut_pump(){
    HAL_GPIO_WritePin(PUMP_PORT, PUMP_PIN, GPIO_PIN_SET);
    stop_create_water_timer();          //关闭计费定时器
}

void start_create_water(){
    if(device_status.boot == 0){
        stop_create_water();
        return ;
    }
    open_create_water(); 
    open_pump();
}

void stop_create_water(){
    shut_create_water();
    shut_pump();
}

/*----------------------------------------------------------------
| Function    :    start_rinse
| Description :    开始冲洗
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void start_rinse(){
    if(device_status.boot == 0){
        stop_rinse();
        return ;
    }
    start_create_water(); 
    open_rinse();
}

/*----------------------------------------------------------------
| Function    :    status_network_led
| Description :    设置网络指示灯
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void status_network_led(GPIO_PinState status){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, status);
}

/*----------------------------------------------------------------
| Function    :    status_error_led
| Description :    设置错误指示灯
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void status_error_led(GPIO_PinState status){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, status);
}

/*----------------------------------------------------------------
| Function    :    start_rinse
| Description :    停止冲洗
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void stop_rinse(){
    stop_create_water();  
    shut_rinse();
}
 
/*----------------------------------------------------------------
| Function    :    open_wdi
| Description :    开启看门狗
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void open_wdi(){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}

/*----------------------------------------------------------------
| Function    :    close_wdi
| Description :    关闭看门狗
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void close_wdi(){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

/*----------------------------------------------------------------
| Function    :    feed_wdi
| Description :    喂狗
| Input       :    null
| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void feed_wdi(){
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
}

/*----------------------------------------------------------------
| Function    :    beep
| Description :    蜂鸣器 响 多少ms

| Input       :        uint8_t ms

| Output      :    null
| Return      :    null
----------------------------------------------------------------*/
void beep(uint8_t ms){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    osDelay(ms);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
}