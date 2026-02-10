/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "box_ctrl.h"
#include "LED.h"
#include "myUsart.h"
#include "OLED.h"
#include "protocol.h"
#include "sensor.h"
#include "servo.h"
#include "usart.h"
#include "OLED_Task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for BoxCtrlTask */
osThreadId_t BoxCtrlTaskHandle;
const osThreadAttr_t BoxCtrlTask_attributes = {
  .name = "BoxCtrlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for BluetoothTask */
osThreadId_t BluetoothTaskHandle;
const osThreadAttr_t BluetoothTask_attributes = {
  .name = "BluetoothTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OLEDTask */
osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for OLEDQueue */
osMessageQueueId_t OLEDQueueHandle;
const osMessageQueueAttr_t OLEDQueue_attributes = {
  .name = "OLEDQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartBoxCtrlTask(void *argument);
void StartBluetoothTask(void *argument);
void StartOledTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
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

  /* Create the queue(s) */
  /* creation of OLEDQueue */
  OLEDQueueHandle = osMessageQueueNew (8, sizeof(OLED_Msg_t), &OLEDQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of BoxCtrlTask */
  BoxCtrlTaskHandle = osThreadNew(StartBoxCtrlTask, NULL, &BoxCtrlTask_attributes);

  /* creation of BluetoothTask */
  BluetoothTaskHandle = osThreadNew(StartBluetoothTask, NULL, &BluetoothTask_attributes);

  /* creation of OLEDTask */
  OLEDTaskHandle = osThreadNew(StartOledTask, NULL, &OLEDTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartBoxCtrlTask */
/**
  * @brief  Function implementing the BoxCtrlTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartBoxCtrlTask */
void StartBoxCtrlTask(void *argument)
{
  /* USER CODE BEGIN StartBoxCtrlTask */
  BoxCtrl_Init();   // ⚠️ 上电初始化，只做一次
  OLED_Clear();
  /* Infinite loop */
  for(;;)
  {
    Box_Process();   // ⭐ 状态机核心
    osDelay(10);     // 10ms tick（100Hz）
  }
  /* USER CODE END StartBoxCtrlTask */
}

/* USER CODE BEGIN Header_StartBluetoothTask */
/**
* @brief Function implementing the BluetoothTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBluetoothTask */
void StartBluetoothTask(void *argument)
{
  /* USER CODE BEGIN StartBluetoothTask */
    Bluetooth_Init();

  /* Infinite loop */
  for(;;)
  {
    if (cmd_ready)
    {
      Protocol_Parse(rx_buf);   // 直接交给协议层
      memset(rx_buf, 0, RX_BUF_SIZE); // 清空缓冲，防止下次干扰
      cmd_ready = 0;
    }
    osDelay(20);
  }
  /* USER CODE END StartBluetoothTask */
}

/* USER CODE BEGIN Header_StartOledTask */
/**
* @brief Function implementing the OLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOledTask */
void StartOledTask(void *argument)
{
  /* USER CODE BEGIN StartOledTask */
  OLED_Init();   // 只初始化一次
  OLED_Msg_t msg;
  /* Infinite loop */
  for(;;)
  {
    // 阻塞等待显示请求
    if (osMessageQueueGet(OLEDQueueHandle, &msg, NULL, osWaitForever) == osOK)
    {


      switch (msg.type)
      {
      case OLED_SHOW_OPEN:
        OLED_Clear();
        OLED_ShowNum(1, 1, msg.box_id, 1);
        OLED_ShowChinese(16, 0, "号");
        OLED_ShowChinese(32, 0, "开");
        break;

      case OLED_SHOW_CLOSE:
        OLED_Clear();
        OLED_ShowNum(1, 1, msg.box_id, 1);
        OLED_ShowChinese(16, 0, "号");
        OLED_ShowChinese(32, 0, "关");
        break;

      case OLED_SHOW_TAKE:
        OLED_Clear();
        OLED_ShowNum(1, 1, msg.box_id, 1);
        OLED_ShowChinese(20, 0, "号");
        OLED_ShowChinese(36, 0, "请");
        OLED_ShowChinese(52, 0, "取");
        OLED_ShowChinese(68, 0, "药");
        break;

      case OLED_SHOW_TAKEN:
        OLED_Clear();
        OLED_ShowChinese(0, 0, "已");
        OLED_ShowChinese(17, 0, "取");
        OLED_ShowChinese(34, 0, "药");
        break;

      default:
        break;
      }
    }
  }
  /* USER CODE END StartOledTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

