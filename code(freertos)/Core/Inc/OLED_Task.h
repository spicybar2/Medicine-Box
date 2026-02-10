
#ifndef P1_FIRSTTASK_OLED_TASK_H
#define P1_FIRSTTASK_OLED_TASK_H
#include <stdint.h>
#include "cmsis_os2.h"

typedef enum
{
    OLED_SHOW_IDLE,
    OLED_SHOW_OPEN,
    OLED_SHOW_CLOSE,
    OLED_SHOW_TAKE,
    OLED_SHOW_TAKEN
} OLED_MsgType_t;

typedef struct
{
    OLED_MsgType_t type;
    uint8_t box_id;
} OLED_Msg_t;

extern osMessageQueueId_t OLEDQueueHandle;

#endif //P1_FIRSTTASK_OLED_TASK_H