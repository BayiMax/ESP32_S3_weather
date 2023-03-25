/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-31 19:57:14
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 15:23:30
 * @FilePath: \esps3_V1\components\TFT\src\weather.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include "Watch.h"

lv_style_t weather_22Font; // 创建22样式
lv_style_t weather_16Font; // 创建16样式

lv_obj_t *location_txt; // 地区组件
lv_obj_t *timeShow;     // 时间组件
lv_obj_t *Power_Show;

lv_obj_t *dateShow; // 日期组件
lv_obj_t *temperature;
lv_obj_t *text_day;
lv_obj_t *text_night;
lv_obj_t *rainfall;
lv_obj_t *humidity;
lv_obj_t *wind_direction;
lv_obj_t *wind_direction_degree;
lv_obj_t *wind_speed;
lv_obj_t *wind_scale;

static void Refresh_Power_Show(lv_task_t *t);
/**
 * @brief 顶部显示框
 *
 * @param Top 样式
 */
static void This_Top(lv_obj_t *Top)
{
  lv_obj_t *Down_btn = lv_label_create(Top, NULL);
  lv_obj_add_style(Down_btn, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(Down_btn, LV_SYMBOL_DOWN);
  lv_obj_align(Down_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

  lv_obj_t *wifi;
  wifi = lv_label_create(Top, NULL);
  lv_obj_add_style(wifi, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(wifi, LV_SYMBOL_WIFI);
  lv_obj_align(wifi, Down_btn, LV_ALIGN_OUT_LEFT_MID, -67, -5);

  lv_obj_t *BLE;
  BLE = lv_label_create(Top, NULL);
  lv_obj_add_style(BLE, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(BLE, LV_SYMBOL_BLUETOOTH);
  lv_obj_align(BLE, wifi, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

  lv_obj_t *POWER;
  POWER = lv_label_create(Top, NULL);
  lv_obj_add_style(POWER, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(POWER, LV_SYMBOL_BATTERY_FULL);
  lv_obj_align(POWER, Down_btn, LV_ALIGN_OUT_RIGHT_MID, 67, -5);

  Power_Show = lv_label_create(Top, NULL);
  lv_obj_add_style(Power_Show, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(Power_Show, "0.00");
  lv_obj_align(Power_Show, POWER, LV_ALIGN_OUT_LEFT_MID, -3, 0);

  lv_task_create(Refresh_Power_Show, 1000, LV_TASK_PRIO_LOW, NULL);
}
/**
 * @brief
 *
 * @param t
 */
static void Refresh_Power_Show(lv_task_t *t)
{
  char Text_buf[10];
  sprintf(Text_buf, "%.2f", Power_C.BAT_Value);
  lv_label_set_text(Power_Show, Text_buf);
}
/**
 * @brief 次顶部显示框
 * 时间,日期,定位
 *
 * @param Top_two 样式
 */
static void This_Toptwo(lv_obj_t *Top_two)
{
  timeShow = lv_label_create(Top_two, NULL);
  lv_obj_add_style(timeShow, LV_LABEL_PART_MAIN, &weather_22Font); // 为对象添加样式
  lv_label_set_text(timeShow, "00:00:00");
  lv_obj_align(timeShow, NULL, LV_ALIGN_IN_TOP_MID, 0, 32);

  dateShow = lv_label_create(Top_two, NULL);
  lv_obj_add_style(dateShow, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(dateShow, "00-00");
  lv_obj_align(dateShow, timeShow, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

  location_txt = lv_label_create(Top_two, NULL);
  lv_obj_add_style(location_txt, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
  lv_label_set_text(location_txt, "CQ");
  lv_obj_align(location_txt, timeShow, LV_ALIGN_OUT_LEFT_MID, -8, 0);

  lv_obj_t *location_icon = lv_label_create(Top_two, NULL);
  lv_obj_add_style(location_icon, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式

  lv_obj_set_style_local_text_color(location_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xc0c0c0));

  lv_label_set_text(location_icon, LV_SYMBOL_GPS);
  lv_obj_align(location_icon, location_txt, LV_ALIGN_OUT_LEFT_MID, -8, 0);
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
  lv_obj_align(timeShow, NULL, LV_ALIGN_IN_TOP_MID, 0, 32);
  lv_label_set_text(timeShow, time_buf);
}
/**
 * @brief
 *
 * @param data
 */
static void Refresh_Weather_Once(lv_task_t *data)
{
  char text_buf[40];

  strncpy(text_buf, WData_All.today.date + 5, 5);
  lv_label_set_text(dateShow, text_buf);
  sprintf(text_buf, "%s'~%s'", WData_All.today.high, WData_All.today.low);
  lv_obj_align(temperature, NULL, LV_ALIGN_IN_TOP_MID, 0, 70);
  lv_label_set_text(temperature, text_buf);

  // sprintf(text_buf, "%s", WData_All.today.text_day);
  lv_label_set_text(text_day, WData_All.today.text_day);
  // sprintf(text_buf, "%s", WData_All.today.text_night);
  lv_label_set_text(text_night, WData_All.today.text_night);

  sprintf(text_buf, "%smm", WData_All.today.rainfall);
  lv_label_set_text(rainfall, text_buf);
  sprintf(text_buf, "%s%%", WData_All.today.humidity);
  lv_label_set_text(humidity, text_buf);

  lv_label_set_text(wind_direction, WData_All.today.wind_direction);
  sprintf(text_buf, "%s'", WData_All.today.wind_direction_degree);
  lv_label_set_text(wind_direction_degree, text_buf);

  sprintf(text_buf, "%skm/h", WData_All.today.wind_speed);
  lv_label_set_text(wind_speed, text_buf);
  lv_label_set_text(wind_scale, WData_All.today.wind_scale);
}
/**
 * @brief 创建单次任务以更新天气数据
 *
 */
void Refresh_Weather_test(void)
{
  static lv_task_t *R_W_T;
  R_W_T = lv_task_create(Refresh_Weather_Once, 500, LV_TASK_PRIO_MID, NULL);
  lv_task_once(R_W_T);
}
/**
 * @brief 天气页面主显示框
 *
 * @param show_in 样式
 */
static void This_show_in(lv_obj_t *show_in)
{
  temperature = lv_label_create(show_in, NULL);
  lv_obj_add_style(temperature, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(temperature, "H:00'~L:00'");
  lv_obj_align(temperature, NULL, LV_ALIGN_IN_TOP_MID, 0, 70);

  text_day = lv_label_create(show_in, NULL);
  lv_obj_add_style(text_day, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(text_day, "xxxxxx");
  lv_obj_align(text_day, NULL, LV_ALIGN_IN_TOP_MID, -60, 104);

  text_night = lv_label_create(show_in, NULL);
  lv_obj_add_style(text_night, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(text_night, "xxxxxx");
  lv_obj_align(text_night, NULL, LV_ALIGN_IN_TOP_MID, 60, 104);

  rainfall = lv_label_create(show_in, NULL);
  lv_obj_add_style(rainfall, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(rainfall, "0.00mm");
  lv_obj_align(rainfall, text_day, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  humidity = lv_label_create(show_in, NULL);
  lv_obj_add_style(humidity, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(humidity, "00%");
  lv_obj_align(humidity, text_night, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  wind_direction = lv_label_create(show_in, NULL);
  lv_obj_add_style(wind_direction, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(wind_direction, "NW");
  lv_obj_align(wind_direction, rainfall, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  wind_direction_degree = lv_label_create(show_in, NULL);
  lv_obj_add_style(wind_direction_degree, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(wind_direction_degree, "000'");
  lv_obj_align(wind_direction_degree, humidity, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  wind_speed = lv_label_create(show_in, NULL);
  lv_obj_add_style(wind_speed, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(wind_speed, "0.0km/h");
  lv_obj_align(wind_speed, wind_direction, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  wind_scale = lv_label_create(show_in, NULL);
  lv_obj_add_style(wind_scale, LV_LABEL_PART_MAIN, &weather_22Font);
  lv_label_set_text(wind_scale, "0");
  lv_obj_align(wind_scale, wind_direction_degree, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);
}

/**
 * @brief 天气页面
 *
 * @param page 父对象
 */
void Weather_page(lv_obj_t *page)
{
  lv_style_init(&weather_22Font); // 初始化样式
  lv_style_set_text_font(&weather_22Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_22); // 设置字体
  lv_style_set_text_color(&weather_22Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  lv_style_init(&weather_16Font); // 初始化样式
  lv_style_set_text_font(&weather_16Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_16); // 设置字体
  lv_style_set_text_color(&weather_16Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  lv_style_init(&weather_16Font); // 初始化样式
  lv_style_set_text_font(&weather_16Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_16); // 设置字体
  lv_style_set_text_color(&weather_16Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  This_Top(page);
  This_Toptwo(page);
  This_show_in(page);
  Timer_Calculate_Init();
  lv_task_create(Refresh_Time, 800, LV_TASK_PRIO_MID, NULL);
}
