/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 00:45:17
 * @FilePath: \esps3_V1\components\TFT\include\Watch.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef Watch_h
#define Watch_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
// #include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "lvgl.h"
#include "lvgl_helpers.h"

#include "main.h"
#include "esp_wifi.h"
#include "User_img.h"
#include "Http_Weather.h"
#include "Http_time.h"
#include "User_Wifi.h"
#include "web_serve.h"
#include "power.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lv_obj_t *time;                  //时间
// lv_obj_t *date;                  //日期
// lv_obj_t *text_day;              //白天天气现象
// lv_obj_t *text_night;            //夜晚天气现象
// lv_obj_t *high;                  //最高温度
// lv_obj_t *low;                   //最低温度
// lv_obj_t *rainfall;              //降雨量
// lv_obj_t *wind_direction;        //风向文字
// lv_obj_t *wind_direction_degree; //风向角
// lv_obj_t *wind_speed;            //风速
// lv_obj_t *wind_scale;            //风力等级
// lv_obj_t *humidity;              //相对湿度

/*watch 定义*/
#define U_BACK_COLOR lv_color_hex(0x140d36)

enum Button_flag
{
  trigger_open = 0x01,
  trigger_close,
  trigger_attonity,
};

void lv_Watch_theme(void);

void User_Gui_Test(void);

void Refresh_Weather_test(void);

void Watch_test(void *pvParameters);

/*weather页面*/
void Weather_page(lv_obj_t *page);
/*weather页面结束*/

/*outswitch页面*/
void WB_Set_Page(lv_obj_t *page);
/*outswitch页面结束*/

/*表界面*/
typedef struct
{
  char hour;
  char minute;
  char second;
} _TYP_TIME;
extern _TYP_TIME U_time;
void Timer_Calculate_Init(void);
void Time_TO_HttpTime(void);
void Refresh_timeDataOnce_test(void);
void Time_Page(lv_obj_t *obj);
/*表界面 OVER*/

/*outswitch界面*/
void OutSwitch_tast(void *pvParameters);
typedef struct
{
  volatile char Wifi_Flag;
  volatile char Ble_Flag;
} _OUTSWITCH;
extern _OUTSWITCH Out_Switch;
/**/
#endif /* Watch_h */
