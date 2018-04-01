#include "bc95.h"
#include <stdlib.h>

/*
|---------------------------------- 
|   bc95 init command   
|----------------------------------
*/
uint8_t cmd_check[]                 = "AT\r\n";             /*!<  OK */
uint8_t cmd_unset_fully_funtion[]   = "AT+CFUN=0\r\n";      /*!<  OK */
uint8_t cmd_unset_back[]            = "AT+CMEE=1\r\n";      /*!<  OK */
uint8_t cmd_unset_psm[]             = "AT+CPSMS=0\r\n";     /*!<  OK */
uint8_t cmd_set_pdp[]               = "AT+CGATT=1\r\n";     /*!<  OK */

uint8_t cmd_get_profile_status[]    = "AT+CGATT?\r\n";      /*!<  +CGATT:1  OK               */
uint8_t cmd_get_imsi[]              = "AT+CIMI\r\n";        /*!<  460111174587916   OK       */
uint8_t cmd_get_imei[]              = "AT+CGSN=1\r\n";      /*!<  +CGSN:863703030636570   OK */
uint8_t cmd_get_csq[]               = "AT+CSQ\r\n";         /*!<  +CSQ:22,99    OK           */ 
uint8_t cmd_get_EPS_status[]        = "AT+CEREG?\r\n";      /*!<  +CEREG:0,1    OK           */
uint8_t cmd_get_iccid[]             = "AT+NCCID?\r\n";      /*!<  +NCCID:*/

uint8_t cmd_check_recv[]            = "AT+NQMGR\r\n";               

/*--------------------------------
    query send or recv number
---------------------------------*/
uint8_t cmd_query_send[]            = "AT+NQMGS\r\n";       /*!<  PENDING=0,SENT=3,ERROR=0        */
uint8_t cmd_query_recv[]            = "AT+NQMGR\r\n";       /*!<  BUFFERED=0,RECEIVED=0,DROPPED=0 */

/*
|---------------------------------- 
|   bc95 coap command   
|----------------------------------
*/
uint8_t cmd_set_sended_flag[]       = "AT+NSMI=1\r\n";      /*!<  OK     +NSMI:SENT  
                                                                  on use coap send a message    */
uint8_t cmd_set_recv_flag[]         = "AT+NNMI=2\r\n";      /*!<  +NNMI              
                                                                  if UE recv a message */

uint8_t cmd_set_pdp_context[]       = "AT+CGDCONT=1,\"IP\",\"psm0.eDRX0.ctnb\"\r\n";
                                                            /*!<  OK                            */

uint8_t cmd_get_server_ip[]         = "AT+NCDP?\r\n";       /*!<  +NCDP:180.101.147.115,5683    */

uint8_t cmd_coap_get_data[]         = "AT+NMGR\r\n";

    /*!<  if the get server ip is not need ip , run the following command                       */
    uint8_t cmd_disconnect[]            = "AT+CFUN=0\r\n";      /*!<  OK                        */
    uint8_t cmd_set_server_ip[]         = "AT+NCDP=117.60.157.137,5683\r\n";
    
                                                                /*!<  OK                        */
    uint8_t cmd_set_fully_funtion[]     = "AT+CFUN=1\r\n";      /*!<  OK 
                                                                      need  long time           */
    uint8_t cmd_reboot[]                = "AT+NRB\r\n";         /*!<  reboot bc95               */
/*
|---------------------------------- 
|   bc95 set command return check
|----------------------------------
*/
uint8_t cmd_ok[]              = "OK";
uint8_t cmd_error[]           = "ERROR";
uint8_t cmd_coap_send_ok[11]  = "+NSMI:SENT";

uint8_t cmd_coap_have_recv[]  = "+NNMI";

uint8_t server_ip[]           = "117.60.157.137,5683";
uint8_t header[]              = "BB666680";

/*
|---------------------------------- 
|   bc95 status typedef
|----------------------------------
*/
bc95_recvTypeDef bc95_recv;
bc95_statusTypeDef bc95_status;

/*----------------------------------------------------------------
 | Function    :    bc95_init
 | Description :    init bc95 and connect network
 | Input       :    null
 | Output      :    
 | Return      :    null
----------------------------------------------------------------*/
void bc95_init(){

    device_error.bc95_init_error = 0;
    device_error.bc95_init_error += bc95_send_command(cmd_check             , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    if(device_error.bc95_init_error == 0){
        //如果未检测到bc95 则设置bc95错误标志位，并挂起当前任务
        device_error.bc95_init_error = BC95_CONNECT_ERROR;
        vTaskSuspend(0);
    }
    //device_error.bc95_init_error += bc95_send_command(cmd_unset_fully_funtion,cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    device_error.bc95_init_error += bc95_send_command(cmd_unset_back        , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    device_error.bc95_init_error += bc95_send_command(cmd_unset_psm         , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);

    device_error.bc95_init_error += bc95_get_imei();

    //device_error.bc95_init_error += bc95_send_command(cmd_set_fully_funtion , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    device_error.bc95_init_error += bc95_send_command(cmd_set_pdp_context   , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    device_error.bc95_init_error += bc95_send_command(cmd_set_pdp           , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);    
    
    device_error.bc95_init_error += bc95_get_iccid();
    device_error.bc95_init_error += bc95_get_imsi();
    device_error.bc95_init_error += bc95_get_profile_status();
    device_error.bc95_init_error += bc95_get_csq();
    device_error.bc95_init_error += bc95_set_server_ip();
    device_error.bc95_init_error += bc95_send_command(cmd_set_recv_flag     , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    device_error.bc95_init_error += bc95_send_command(cmd_set_sended_flag   , cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    if(device_error.bc95_init_error != 13){         // if bc95 init have error than reboot bc95 and re init
        device_error.bc95_init_error = 1;
        bc95_reboot();
        return;
    }else{
        device_error.bc95_init_error = 0;
        if(registe_device()){
            device_status.bc95_run = 1;
        }else{
            bc95_reboot();
        }
        
    }
}
/*----------------------------------------------------------------
 | Function    :    bc95_send_command
 | Description :    usart send data to bc95

 | Input       :        uint8_t *cmd        command
                        uint8_t *ack        need return string
                        uint8_t timeout     second
                        
 | Output      :    null
 | Return      :        uint8_t     OK --> 1      ERROR --> 0
----------------------------------------------------------------*/
uint8_t bc95_send_command(uint8_t *cmd, uint8_t *ack, uint8_t timeout, uint8_t loop){
    do{
        bc95_open_recv();
        //use usart2 send command
        HAL_UART_Transmit_DMA(&huart2, cmd, (uint8_t)strlen((const char*)cmd));
        #ifdef DEBUG
        printf("usart2 send command: %s", cmd);
        #endif
        uint32_t time_count = (timeout*1000)/BC95_CMD_DELAY;    // Calculation loop number
        while(time_count--){      
            osDelay(BC95_CMD_DELAY);
            if(bc95_recv.rx_flag){                              // check recv flag
                if(bc95_check_ack(ack)){                        // check recv data
                    #ifdef DEBUG
                    printf("SUCCESS: check ack ok \r\n\r\n");
                    #endif
                    _network_led;
                    return OK;
                }else{

                }
            }
        }
        _CLEAR_RX_TYPEDEF;                                      // clear all rx data
    }while(loop--);                                             // loop number
    return ERROR;
}

/*----------------------------------------------------------------
 | Function    :    bc95_open_recv
 | Description :    set bc95_RecvTypeDef empty and start usart dma receive
 | Input       :    null
 | Output      :    null
 | Return      :    null
----------------------------------------------------------------*/
void bc95_open_recv(){
    _CLEAR_RX_TYPEDEF;
    HAL_UART_Receive_DMA(&huart2, bc95_recv.rx_buf, RX_LEN);
}

/*----------------------------------------------------------------
 | Function    :    bc95_check_ack
 | Description :    check bc95 break message
 |
 | Input       :        uint8_t *ack
 |
 | Output      :    null
 |
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_check_ack(uint8_t *ack){
    _network_led;
    if(strstr((const char*)bc95_recv.rx_buf, (const char*)ack) != NULL){
        return  1;
    }else{
        return  0;
    }
}

/*----------------------------------------------------------------
 | Function    :    uint8_t bc95_get_imsi
 | Description :    get bc95 imsi
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_get_imsi(){
    uint8_t count = BC95_LOOP_NUMBER;
    do{
        if(bc95_send_command(cmd_get_imsi, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
            if(strlen((const char*)(const char*)bc95_recv.rx_buf) != 25){
                #ifdef DEBUG
                printf("ERROR:rx buffer length error\r\n");
                #endif
                continue;
            }
            memmove(bc95_status.imsi, &bc95_recv.rx_buf[2], 15);    // copy imsi to bc95_status.imsi
            if(strlen((const char*)bc95_status.imsi) == 15){                     // judge imsi length
                #ifdef DEBUG
                printf("SUCCESS\r\n");
                printf("imsi:%s\r\n\r\n", bc95_status.imsi);
                #endif
                return OK;
            }else{
                #ifdef DEBUG    
                printf("ERROR:imsi number error\r\n");
                #endif
                continue;
            }
        }else{
            #ifdef DEBUG
            printf("ERROR:command exec error\r\n");
            #endif
            continue;
        }
    }while(count--);
    return ERROR;
}

/*----------------------------------------------------------------
 | Function    :    uint8_t bc95_get_imei
 | Description :    get bc95 imei
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_get_imei(){
    uint8_t count = BC95_LOOP_NUMBER;
    do{     
        if(bc95_send_command(cmd_get_imei, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
            if(strlen((const char*)bc95_recv.rx_buf) != 31){
                #ifdef DEBUG
                printf("ERROR:rx buffer length error\r\n");
                #endif
                continue;
            }
            memmove(bc95_status.imei, &bc95_recv.rx_buf[8], 15);    // copy imsi to bc95_status.imsi
            if(strlen((const char*)bc95_status.imei) == 15){                     // judge imsi length
                #ifdef DEBUG
                printf("SUCCESS\r\n");
                printf("imei:%s\r\n\r\n", bc95_status.imei);
                #endif
                return OK;
            }else{
                #ifdef DEBUG
                printf("ERROR:imei number error\r\n");
                #endif
                continue;
            }
        }else{
            #ifdef DEBUG
            printf("ERROR:command exec error\r\n");
            #endif
            continue;
        }
    }while(count--);
    return ERROR;
}
/*----------------------------------------------------------------
 | Function    :    uint8_t bc95_get_iccid
 | Description :    get bc95 iccid
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_get_iccid(){
    uint8_t count = BC95_LOOP_NUMBER;
    do{
        if(bc95_send_command(cmd_get_iccid, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
            if(strlen((const char*)bc95_recv.rx_buf) != 37){
                #ifdef DEBUG
                printf("ERROR:rx buffer length error\r\n");
                #endif
                continue;
            }
            memmove(bc95_status.iccid, &bc95_recv.rx_buf[9], 19);    // copy imsi to bc95_status.imsi
            if(strlen((const char*)bc95_status.iccid) == 19){      // judge imsi length
                #ifdef DEBUG
                printf("SUCCESS\r\n");
                printf("iccid:%s\r\n\r\n", bc95_status.iccid);
                #endif
                return OK;
            }else{
                #ifdef DEBUG
                printf("ERROR:iccid number error\r\n");
                #endif
                continue;
            }
        }else{
            #ifdef DEBUG
            printf("ERROR:command exec error\r\n");
            #endif
            continue;
        }
    }while(count--);
    return ERROR;
}
/*----------------------------------------------------------------
 | Function    :    bc95_get_profile_status
 | Description :    get bc95 profile status check in network
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_get_profile_status(){
    uint8_t count = BC95_LOOP_NUMBER * 2;
    do{
        if(bc95_send_command(cmd_get_profile_status, "+CGATT:1", BC95_TIMEOUT, BC95_LOOP_NUMBER)){
            bc95_status.profile_status = 1;
            #ifdef DEBUG
            printf("SUCCESS\r\n");
            printf("profile status: %d\r\n\r\n", bc95_status.profile_status);
            #endif
            return OK; 
        }   
        #ifdef DEBUG
        printf("ERROR:UE no network\r\n");
        #endif
        bc95_status.profile_status = 0;
    }while(count--);
    #ifdef DEBUG
    printf("ERROR:get profile status time out\r\n");
    #endif
    return ERROR;
}

/*----------------------------------------------------------------
 | Function    :    bc95_get_csq
 | Description :    get bc95 csq number and save in the bc95_status struct
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_get_csq(){
    uint8_t count = BC95_LOOP_NUMBER;
    uint8_t cache[3] = {0};
    do{
        if(bc95_send_command(cmd_get_csq, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
            char *begin = strchr((const char*)bc95_recv.rx_buf, ':') + 1;
            char *end   = strchr((const char*)bc95_recv.rx_buf, ',');
            uint8_t  csq_len = end - begin;
            memmove(cache, begin, csq_len);
            if((bc95_status.csq = atoi((const char*)cache)) > 32){
                #ifdef DEBUG
                printf("ERROR:csq number error\r\n");
                #endif
                bc95_status.csq = 0;
                continue;
            }
            #ifdef DEBUG
            printf("SUCCESS\r\n");
            printf("csq: %d\r\n", bc95_status.csq);
            #endif
            return OK;
        }
    }while(count--);
    #ifdef DEBUG
    printf("ERROR:get csq time out\r\n");
    #endif
    return ERROR;
}

/*----------------------------------------------------------------
 | Function    :    bc95_set_server_ip
 | Description :    first get server ip then judge ,if is not the server ip then set 
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_set_server_ip(){
    if(bc95_send_command(cmd_get_server_ip, server_ip, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
        #ifdef DEBUG
        printf("SUCCESS: server ip is setted\r\n");
        #endif
        return OK; 
    }
    bc95_send_command(cmd_disconnect, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER);
    uint8_t cmd[30] = "AT+NCDP=";
    strcat((char*)cmd, (const char*)server_ip);
    strcat((char*)cmd, "\r\n");
    if(bc95_send_command(cmd, cmd_ok, BC95_TIMEOUT, BC95_LOOP_NUMBER)){
        if(bc95_send_command(cmd_set_fully_funtion, cmd_ok, BC95_TIMEOUT * 3, BC95_LOOP_NUMBER)){
            #ifdef DEBUG
            printf("SUCCESS: server ip (%s) be set \r\n", server_ip);
            #endif
            bc95_reboot();
        }
    }else{
        #ifdef DEBUG
        printf("ERROR: set server ip timeout\r\n");
        #endif
        bc95_reboot();
    }
    #ifdef DEBUG
    printf("ERROR: set server ip error\r\n");
    #endif
    return ERROR;
}

/*----------------------------------------------------------------
 | Function    :    bc95_reboot
 | Description :    first send reboot command ,and init bc95 mode
 | Input       :    null
 | Output      :    null
 | Return      :    null
----------------------------------------------------------------*/
void bc95_reboot(){
    #ifdef DEBUG
    printf("bc95 rebooting\r\n");
    #endif
    //bc95_send_command(cmd_reboot, "REBOOT", DISABLE, DISABLE);
    HAL_GPIO_WritePin(BC95_RESET_PORT, BC95_RESET_PIN, GPIO_PIN_SET);
    osDelay(150);
    HAL_GPIO_WritePin(BC95_RESET_PORT, BC95_RESET_PIN, GPIO_PIN_RESET);
    bc95_init();
}

/*----------------------------------------------------------------
 | Function    :    bc95_send_coap
 | Description :    向服务器发送所有数据
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_send_coap(uint8_t *ack){
    uint8_t cmd_coap_head[30] = "AT+NMGS=48,";
    uint16_t j = 0;
    uint8_t cmd[300] = {0};

    j += sprintf((char*)cmd, "%s%02x",cmd_coap_head ,device_status.device_registe);

    strcat((char*)cmd, (const char*)ByteToHexStr(bc95_status.iccid, strlen((const char*)bc95_status.iccid)));
    j += strlen((const char*)bc95_status.iccid)*2;
    j += sprintf((char*)cmd+j, "%02X", device_status.boot);
    j += sprintf((char*)cmd+j, "%04X%04X", device_status.raw_tds, device_status.pure_tds);
    j += sprintf((char*)cmd+j, "%08X%08X", device_status.create_water_time_m , device_status.flow);
    j += sprintf((char*)cmd+j, "%02X%04X", bc95_status.csq      , bc95_status.snr);
    j += sprintf((char*)cmd+j, "%02X%02X%02X", device_error.leakage, device_error.raw_no_water, device_error.create_water_too_long);
    j += sprintf((char*)cmd+j, "%02X", device_status.processing_status);
    strcat((char*)cmd, (char*)ByteToHexStr(header, strlen((const char*)header)));
    strcat((char*)cmd, "\r\n");

    if(bc95_send_command(cmd, ack, 30, BC95_LOOP_NUMBER)){
        #ifdef DEBUG    
        printf("SUCCESS:send data to server success\r\n");
        #endif
        return OK;
        
    }else{
        #ifdef DEBUG
        printf("ERROR:send data to server error\r\n");
        #endif
        return ERROR;
   }
}

/*----------------------------------------------------------------
 | Function    :    bc95_read_coap
 | Description :    读BC95接收到的数据
 | Input       :    null
 | Output      :    null
 | Return      :    null
----------------------------------------------------------------*/
void bc95_read_coap(uint8_t timeout){
    #ifdef DEBUG
    printf("INFO: bc95 have a coap recv\r\n");
    #endif
    bc95_open_recv();
    HAL_UART_Transmit_DMA(&huart2, cmd_coap_get_data, strlen((const char*)cmd_coap_get_data));
    do{
        osDelay(BC95_CMD_DELAY);

        if(bc95_check_ack(cmd_ok)){                          // check recv data
            #ifdef DEBUG    
            printf("SUCCESS: check ack ok \r\n\r\n");
            #endif
            char *begin = strchr((const char*)bc95_recv.rx_buf, ',') + 1;
            /*!<  copy server command to bc95_recv.server_cmd than */
            memmove(bc95_recv.server_cmd, begin, (SERVER_CMD_LEN * 2));
            processing_server_command();
            return;
        }else{
            #ifdef DEBUG
            printf("ERROR");
            printf("\r\n----- recv data ---------------\r\n");
            printf("%s", bc95_recv.rx_buf);
            printf("\r\n----- end ---------------------\r\n");
            printf("check ack error ...\r\n\r\n");
            #endif
        }
        
    }while(timeout--);
    bc95_open_recv();
}

/*----------------------------------------------------------------
 | Function    :    bc95_check_recv
 | Description :    检查BC95是否接收到数据
 | Input       :    null
 | Output      :    null
 | Return      :        ok --> 1    error --> 0
----------------------------------------------------------------*/
uint8_t bc95_check_recv(){
    
    uint8_t cache[3] = {0};
    
    if(bc95_send_command(cmd_check_recv, cmd_ok, 1, 1)){
        char *begin = strchr((const char*)bc95_recv.rx_buf, '=') + 1;
        char *end   = strchr((const char*)bc95_recv.rx_buf, ',');
        uint8_t  csq_len = end - begin;
        memmove(cache, begin, csq_len);
        if((bc95_status.recv_buffer = atoi((const char*)cache)) == 0 ){
            return ERROR;
        }
        
        for(uint8_t i = 0 ; i < bc95_status.recv_buffer ; i++){
            bc95_read_coap(BC95_TIMEOUT);
        }
        
        return OK;
    }
    return ERROR;
}
        
/*----------------------------------------------------------------
 | Function    :    bc95_recv_idle_callback
 | Description :    bc95 idle interrupt callback funtion
 |
 | Input       :        UART_HandleTypeDef  *huart
 |
 | Output      :    
 | Return      :    
----------------------------------------------------------------*/
void bc95_recv_idle_callback(UART_HandleTypeDef *huart){
    if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        HAL_UART_DMAStop(&huart2);
        bc95_recv.rx_size = RX_LEN - (uint32_t)huart2.hdmarx->Instance->CNDTR;
        bc95_recv.rx_flag = OK;
        if(strstr((const char*)bc95_recv.rx_buf, (const char*)cmd_coap_have_recv) != NULL){
            bc95_recv.server_cmd_flag = 1;
        }
        HAL_UART_Receive_DMA(&huart2, bc95_recv.rx_buf, RX_LEN);
    }
}