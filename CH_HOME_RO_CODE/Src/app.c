#include "app.h"

device_statusTypeDef device_status = {0};
device_errorTypeDef device_error   = {0};

uint32_t device_adc_value[ADC_CHANNEL_NUMBER*ADC_LOOP_NUMBER-1];


/*----------------------------------------------------------------
 | Function    :    boot_rinse
 | Description :    开机冲洗2分钟
 | Input       :    null
 | Output      :    null
 | Return      :    null
----------------------------------------------------------------*/
void boot_rinse(){
    if(device_status.booting == 0){
        TickType_t StartTime = xTaskGetTickCount();
        start_rinse();
        device_status.booting = 1;
        osDelayUntil(&StartTime, device_status.rinse_time*1000);
        stop_rinse();
    }
}

/*----------------------------------------------------------------
 | Function    :    registe_device
 | Description :    注册设备
 | Input       :    null
 | Output      :    null
 | Return      :    null
----------------------------------------------------------------*/
uint8_t registe_device(){
    if(device_status.device_registe == 1){
        #ifdef DEBUG
        printf("device is registed ...");
        #endif
    }else{
        if(bc95_send_coap("+NNMI") == OK){
            bc95_read_coap(BC95_TIMEOUT);
            return OK;
        }else{
            #ifdef DEBUG
            printf("ERROR:registe error");
            #endif
            return ERROR;
        }
    }
    return ERROR;
}