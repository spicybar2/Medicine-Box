#include "protocol.h"
#include <stdio.h>
#include "string.h"
#include "myUsart.h"
#include "stdlib.h"
#include "box_ctrl.h"
#include "cmsis_os2.h"
#include "OLED_Task.h"

void Protocol_Parse(char *cmd)
{

    if(cmd[0] != '$') {
        Bluetooth_SendString("$ERR,FORMAT\r\n");
        return;
    }

    char *token = strtok(cmd + 1, ",");
    if(token == NULL) return;

    if(strcmp(token, "OPEN") == 0)
    {
        token = strtok(NULL, ",");
        if(token != NULL) {
            uint8_t box_id = atoi(token); // 得到 1, 2 或 3
            BoxCtrl_Open(box_id);         // 传入编号


            OLED_Msg_t msg = {
                .type = OLED_SHOW_OPEN,
                .box_id = box_id
            };
            osMessageQueuePut(OLEDQueueHandle, &msg, 0, 10);

            Bluetooth_SendString("$OK,OPEN\r\n");
        }
    }
    else if(strcmp(token, "CLOSE") == 0)
    {
        token = strtok(NULL, ",");
        if(token != NULL) {
            uint8_t box_id = atoi(token);
            BoxCtrl_Close(box_id);        // 传入编号


            OLED_Msg_t msg = {
                .type = OLED_SHOW_CLOSE,
                .box_id = box_id
            };
            osMessageQueuePut(OLEDQueueHandle, &msg, 0, 10);

            Bluetooth_SendString("$OK,CLOSE\r\n");
        }
    }


    /* 取药流程命令：$TAKE,1 */
    else if(strcmp(token, "TAKE") == 0)
    {
        token = strtok(NULL, ","); // 获取逗号后的数字
        if(token != NULL)
        {
            uint8_t box_id = atoi(token);

            BoxCtrl_StartTake(box_id);   // 传入箱号启动流程


            OLED_Msg_t msg = {
                .type = OLED_SHOW_TAKE,
                .box_id = box_id
            };
            osMessageQueuePut(OLEDQueueHandle, &msg, 0, 0);

            Bluetooth_SendString("$OK,TAKE\r\n");
        }
        else
        {
            Bluetooth_SendString("$ERR,PARAM\r\n");
        }
    }
}
