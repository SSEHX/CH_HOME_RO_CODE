/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include "app.h"

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId AppTaskHandle;
osThreadId Bc95InitTaskHandle;
osThreadId ControlTaskHandle;
osThreadId ErrorCheckTaskHandle;
osThreadId Bc95RecvTaskHandle;
osThreadId Bc95SendTaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartAppTask(void const * argument);
void StartBc95InitTask(void const * argument);
void StartControlTask(void const * argument);
void StartErrorCheckTask(void const * argument);
void StartBc95RecvTask(void const * argument);
void StartBc95SendTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of AppTask */
  osThreadDef(AppTask, StartAppTask, osPriorityHigh, 0, 128);
  AppTaskHandle = osThreadCreate(osThread(AppTask), NULL);

  /* definition and creation of Bc95InitTask */
  osThreadDef(Bc95InitTask, StartBc95InitTask, osPriorityIdle, 0, 800);
  Bc95InitTaskHandle = osThreadCreate(osThread(Bc95InitTask), NULL);

  /* definition and creation of ControlTask */
  osThreadDef(ControlTask, StartControlTask, osPriorityAboveNormal, 0, 128);
  ControlTaskHandle = osThreadCreate(osThread(ControlTask), NULL);

  /* definition and creation of ErrorCheckTask */
  osThreadDef(ErrorCheckTask, StartErrorCheckTask, osPriorityIdle, 0, 256);
  ErrorCheckTaskHandle = osThreadCreate(osThread(ErrorCheckTask), NULL);

  /* definition and creation of Bc95RecvTask */
  osThreadDef(Bc95RecvTask, StartBc95RecvTask, osPriorityRealtime, 0, 800);
  Bc95RecvTaskHandle = osThreadCreate(osThread(Bc95RecvTask), NULL);

  /* definition and creation of Bc95SendTask */
  osThreadDef(Bc95SendTask, StartBc95SendTask, osPriorityIdle, 0, 800);
  Bc95SendTaskHandle = osThreadCreate(osThread(Bc95SendTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartAppTask function */
void StartAppTask(void const * argument)
{

  /* USER CODE BEGIN StartAppTask */
    uint32_t send_data_time_count = 0;
    /* Infinite loop */
    for(;;)
    {
        _system_led;
        
        send_data_time_count++;
        
        if(device_status.device_mode == 1){             //设备为租赁模式
            
            if(device_status.arrears_boot == 0){        //如果设备欠费则停机
                device_status.boot = 0;
            }
            
            if(device_status.flow_or_time == 1){        //流量模式
                
            }else if(device_status.flow_or_time == 2){ //时间模式
                
            }
        }else if(device_status.device_mode == 2){       //设备为零售模式
            //零售模式不存在停机开关机，永远停机开机
            device_status.arrears_boot = 1;      
        }
        
        if(send_data_time_count >= 2880){               //48分钟发送一次
            bc95_status.need_send = 1;
            send_data_time_count = 0;
        }
        osDelay(1000);
    }
  /* USER CODE END StartAppTask */
}

/* StartBc95InitTask function */
void StartBc95InitTask(void const * argument)
{
  /* USER CODE BEGIN StartBc95InitTask */
    /* Infinite loop */
    for(;;)
    {
        if(device_status.bc95_run){
            
            //开始检查bc95接收任务
            /* definition and creation of Bc95RecvTask */
            osThreadDef(Bc95RecvTask, StartBc95RecvTask, osPriorityIdle, 0, 512);
            Bc95RecvTaskHandle = osThreadCreate(osThread(Bc95RecvTask), NULL);
            
            /* definition and creation of ErrorCheckTask */
            osThreadDef(ErrorCheckTask, StartErrorCheckTask, osPriorityIdle, 0, 400);
            ErrorCheckTaskHandle = osThreadCreate(osThread(ErrorCheckTask), NULL);
            
            status_network_led(GPIO_PIN_SET);
            
            //初始化完成后删除该任务
            vTaskDelete(NULL);
        }
        bc95_reboot();
        osDelay(1300);
    }
  /* USER CODE END StartBc95InitTask */
}

/* StartControlTask function */
void StartControlTask(void const * argument)
{
  /* USER CODE BEGIN StartControlTask */
    uint8_t no_water_count = 0;
    /* Infinite loop */
    for(;;)
    {
        //boot_rinse();
        
        //如果需要冲洗，则开启冲洗
        if(device_status.rinse){
            device_status.rinse = 0;
            start_rinse();
            osDelay(device_status.rinse_time * 1000);
            stop_rinse();
        }
        
        //检测是否缺水
        if(get_low_switch()){
            no_water_count++;
            stop_rinse();
            
            if(no_water_count >= 5){    //如果连续10秒没有水
                device_error.raw_no_water = 1;
                
            }
        }else{
            //清空缺水计数
            no_water_count = 0;
            device_error.raw_no_water = 0;
            //允许再次发送缺水信息
            if(device_error.raw_no_water_sended == 1){
                device_status.processing_status = 0;
                bc95_status.need_send = 1;
                device_error.raw_no_water_sended = 0;
            }
            
            //水满，停止制水
            if(get_high_switch()){
                stop_create_water();
            }else{      //水未满，继续制水
                start_create_water();
            }
        }
        osDelay(2000);
    }
  /* USER CODE END StartControlTask */
}

/* StartErrorCheckTask function */
void StartErrorCheckTask(void const * argument)
{
  /* USER CODE BEGIN StartErrorCheckTask */
    
    uint8_t leakage_count = 0;
    
    /* Infinite loop */
    for(;;)
    {
        feed_wdi();     //喂狗
        
        //累加20次漏水检测结果
        uint32_t leakage_value = 0;
        for(uint8_t i = 0 ; i < 20 ; i++){
            leakage_value += adc_value[ i * 3 ];
        }
        /* 计算漏水检测到的数值，并判断是否漏水，如果本次漏水则将漏水计数+1
        如果不漏水则清0， 
        */
        if(leakage_value/20 <= 2500){
            leakage_count++;
        }else{
            leakage_count = 0;
        }
        
        /* 如果连续漏水超过30秒，则设置漏水标志，
        如果不漏水了，则设置可以发送错误标志位，并清除错误
        */
        if(leakage_count >= 6){
            device_error.leakage = 1;
            leakage_count = 6;
            beep(100);
        }else{
            device_error.leakage = 0;
            if(device_error.leakage_sended == 1){
                device_status.processing_status = 0;
                bc95_status.need_send = 1;
                device_error.leakage_sended = 0;
            }
        }
        
        //判断是否有错误,如果有错误，则继续检测是什么错误
        if(device_error.bc95_init_error != 0 ||         // bc95初始化失败
           device_error.create_water_too_long != 0 ||   // 制水时间过长
               device_error.leakage != 0 ||                 // 漏水
                   device_error.raw_no_water != 0 ||            // 原水缺水
                       device_status.arrears_boot == 0 )            // 停机
        {
            
            device_status.processing_status = 3;        //设置错误
            
            //检测BC95是否初始化失败，如果失败则重新初始化
            if(device_error.bc95_init_error == BC95_CONNECT_ERROR){
                beep(50);
                //如果BC95初始化失败，则重新创建初始化任务。
                /* definition and creation of Bc95InitTask */
                osThreadDef(Bc95InitTask, StartBc95InitTask, osPriorityIdle, 0, 512);
                Bc95InitTaskHandle = osThreadCreate(osThread(Bc95InitTask), NULL);
            }
            
            //检测是否已经缺水，如果已经缺水，并且没有发送错误信息，则发送错误
            if(device_error.raw_no_water == 1 && device_error.raw_no_water_sended == 0){
                bc95_status.need_send = 1;
            }
            
            //检测是否发生漏水，如果已经漏水，并且没有发送错误信息，则发送错误信息
            if(device_error.leakage == 1 && device_error.leakage_sended == 0){
                device_status.boot = 0;                 //漏水关机,
                bc95_status.need_send = 1;
            }
            status_error_led(GPIO_PIN_SET);
        }else{
            device_status.processing_status = 0;
            status_error_led(GPIO_PIN_RESET);
        }
        
        osDelay(5000);
    }
  /* USER CODE END StartErrorCheckTask */
}

/* StartBc95RecvTask function */
void StartBc95RecvTask(void const * argument)
{
  /* USER CODE BEGIN StartBc95RecvTask */
    uint8_t count = 0;
    /* Infinite loop */
    for(;;)
    {
        _network_led;
        count++;
        
        if(count >= 5){
            count = 0;
            //获取联网状态，如果掉网则重新注册
            if(!bc95_get_profile_status()){
                bc95_send_command("AT+CGATT=1\r\n", "OK", BC95_TIMEOUT, BC95_LOOP_NUMBER);
            }
            
            bc95_get_csq();
        }
        bc95_check_recv();
        osDelay(3000);
    }
  /* USER CODE END StartBc95RecvTask */
}

/* StartBc95SendTask function */
void StartBc95SendTask(void const * argument)
{
  /* USER CODE BEGIN StartBc95SendTask */
    /* Infinite loop */
    for(;;)
    {
        if(bc95_status.need_send == 1){
            bc95_status.need_send = 0;
            
            if(device_error.leakage_sended == 0 && device_error.leakage == 1){
                device_error.leakage_sended = 1;
                device_status.processing_status = COAP_DEVICE_ERROR;
            }
            
            if(device_error.raw_no_water_sended == 0 && device_error.raw_no_water == 1){
                device_error.raw_no_water_sended = 1;
                device_status.processing_status = COAP_DEVICE_ERROR;
            }
            
            bc95_send_coap(cmd_coap_send_ok);
        }
        osDelay(1000);
    }
  /* USER CODE END StartBc95SendTask */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
