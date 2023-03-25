/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-25 12:45:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 15:07:57
 * @FilePath: \esps3_V1\components\TFT\src\Time.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include "watch.h"
#include "driver/timer.h"

lv_obj_t *time_value;
lv_obj_t *data_value;
/*定时器中断函数*/
static void IRAM_ATTR timer_hdr(void *para);

_TYP_TIME U_time;

uint8_t Timer_calaulate_num = 0;

/**
 * @brief 定时器初始化
 *
 */
void Timer_Calculate_Init(void)
{
  timer_config_t time_config = {
      .divider = 8,                  // 8 分频率
      .counter_dir = TIMER_COUNT_UP, // 计数
      .counter_en = TIMER_PAUSE,     // 初始化后不开始计时
      .alarm_en = TIMER_ALARM_EN,    // 开启中断
      .auto_reload = 1,              // 自动重装预装值
  };
  timer_init(TIMER_GROUP_0, TIMER_0, &time_config); // 初始化定时器
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000 * (TIMER_BASE_CLK / 8 / 1000));
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_hdr, (void *)TIMER_0, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(TIMER_GROUP_0, TIMER_0);
}
/**
 * @brief 从网络更新时间
 *
 */
void Time_TO_HttpTime(void)
{
  uint8_t time_num[6];
  char time_buf[20];
  uint8_t i = 0, x = 0;
  if (Time_Data.Time_Flag == true)
  {
    sprintf(time_buf, "%s", Time_Data.time);
    // MY_DEBUG("%s.", time_buf);
    for (x = 0; x < 20; x++)
    {
      if (time_buf[x] >= '0' && time_buf[x] <= '9')
      {
        time_num[i] = time_buf[x] - '0';
        i++;
      }
    }
    U_time.hour = time_num[0] * 10 + time_num[1];
    U_time.minute = time_num[2] * 10 + time_num[3];
    U_time.second = time_num[4] * 10 + time_num[5];
    // MY_DEBUG("%d:%d:%d.", U_time.hour, U_time.minute, U_time.second);
    Time_Data.Time_Flag = false;
  }
}

static void IRAM_ATTR timer_hdr(void *para)
{
  uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
  if (timer_intr & TIMER_INTR_T0)
  {
    /*清除中断状态*/
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    /*重新使能定时器中断*/
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  }
  U_time.second++;
  if (U_time.second > 59)
  {
    U_time.minute++;
    U_time.second = 0;
  }
  if (U_time.minute > 59)
  {
    U_time.hour++;
    U_time.minute = 0;
  }
  if (U_time.hour > 23)
  {
    U_time.hour = 0;
  }
}

/**
 * @author BBX
 * @date 2022年9月27日10点39分
 * @brief LVGL跟新时间函数
 * @param t lvgl仍任务传入参数
 */
static void Refresh_Time(lv_task_t *t)
{
  char time_buf[20];
  sprintf(time_buf, "%d:%d:%d", U_time.hour, U_time.minute, U_time.second);
  lv_label_set_text(time_value, time_buf);
}
/**
 * @brief
 *
 * @param data
 */
static void Refresh_data_Once(lv_task_t *data)
{
  lv_label_set_text(data_value, WData_All.today.date);
}
/**
 * @brief 创建单次任务以更新天气数据
 *
 */
void Refresh_timeDataOnce_test(void)
{
  static lv_task_t *R_W_T;
  R_W_T = lv_task_create(Refresh_data_Once, 500, LV_TASK_PRIO_MID, NULL);
  lv_task_once(R_W_T);
}
/**
 * @brief
 *
 * @param obj
 */
void Time_Page(lv_obj_t *obj)
{
  static lv_style_t Time_42Font;
  lv_style_init(&Time_42Font); // 初始化样式
  lv_style_set_text_font(&Time_42Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_44); // 设置字体
  lv_style_set_text_color(&Time_42Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  static lv_style_t Time_22Font;
  lv_style_init(&Time_22Font); // 初始化样式
  lv_style_set_text_font(&Time_22Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_22); // 设置字体
  lv_style_set_text_color(&Time_22Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  time_value = lv_label_create(obj, NULL);
  lv_obj_add_style(time_value, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
  lv_label_set_text(time_value, "00:00:00");
  lv_obj_align(time_value, NULL, LV_ALIGN_IN_TOP_MID, 38, 140);

  data_value = lv_label_create(obj, NULL);
  lv_obj_add_style(data_value, LV_LABEL_PART_MAIN, &Time_22Font); // 为对象添加样式
  lv_label_set_text(data_value, "0000-00-00");
  lv_obj_align(data_value, time_value, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_task_create(Refresh_Time, 500, LV_TASK_PRIO_MID, NULL);
}