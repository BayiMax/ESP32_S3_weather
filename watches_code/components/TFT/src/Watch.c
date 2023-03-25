/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 16:27:55
 * @FilePath: \esps3_V1\components\TFT\src\Watch.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include "Watch.h"

static lv_obj_t *Page_weather;
static lv_obj_t *Page_time;
static lv_obj_t *Page_WB_Set_;
static lv_obj_t *Page_Setting;
static lv_obj_t *Page_SDUSB;
volatile char OutWifiSwitch_Flag = 0;

#define LOCK_ON_OFF 0 // 启用锁屏需修改平铺视图坐标

#if LOCK_ON_OFF == 1
/**
 * @brief 锁屏 未使用
 *
 * @param page
 */
void Lock_Page(lv_obj_t *page)
{
  static lv_style_t lcok_font;
  lv_style_init(&lcok_font); // 初始化样式
  lv_style_set_text_font(&lcok_font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_44); // 设置字体
  lv_style_set_text_color(&lcok_font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  lv_obj_t *this_lock_text = lv_label_create(page, NULL);
  lv_obj_add_style(this_lock_text, LV_LABEL_PART_MAIN, &lcok_font); // 为对象添加样式
  lv_label_set_text(this_lock_text, LV_SYMBOL_UP);
  lv_obj_align(this_lock_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 120);
}
#endif

/**
 * @author Bay_Max
 * @date 2022年9月27日10点42分
 * @brief 创建wether界面主题
 * @param 无
 * @return 无
 */
void lv_Watch_theme(void)
{

  static lv_style_t Time_42Font;
  lv_style_init(&Time_42Font); // 初始化样式
  lv_style_set_text_font(&Time_42Font,
                         LV_STATE_DEFAULT, &lv_font_montserrat_44); // 设置字体
  lv_style_set_text_color(&Time_42Font,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  static char User_Page_num = 5;
  static lv_point_t Valid_pos[] = {{0, 1}, {1, 1}, {1, 0}, {2, 1}, {1, 2}};

  lv_obj_t *scr = lv_obj_create(NULL, NULL); // 创建一个基本对象
  lv_scr_load(scr);                          // 激活屏幕

  lv_obj_t *tileview;
  tileview = lv_tileview_create(lv_scr_act(), NULL);
  lv_tileview_set_valid_positions(tileview, Valid_pos, User_Page_num);
  lv_tileview_set_edge_flash(tileview, true);              // 启用边缘闪光效果
  lv_page_set_scrlbar_mode(tileview, LV_SCRLBAR_MODE_OFF); // 取消滚动条

  Page_weather = lv_obj_create(tileview, NULL);            // 创建一个基本对象
  lv_obj_clean_style_list(Page_weather, LV_OBJ_PART_MAIN); // 将样式重置为空
  // lv_obj_set_style_local_radius(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 42);
  lv_obj_set_style_local_bg_opa(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_bg_color(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, U_BACK_COLOR);
  lv_obj_set_size(Page_weather, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_weather, 1 * LV_HOR_RES, 1 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_weather);
  Weather_page(Page_weather);

  Page_time = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
  lv_obj_set_size(Page_time, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_time, 0 * LV_HOR_RES, 1 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_time);
  Time_Page(Page_time);

#if LOCK_ON_OFF == 1
  lv_obj_t *Page_lcok = lv_obj_create(tileview, Page_weather);
  lv_obj_set_size(Page_lcok, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_lcok, 0 * LV_HOR_RES, 0 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_lcok);
  Lock_Page(Page_lcok);
#endif
  Page_WB_Set_ = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
  lv_obj_set_size(Page_WB_Set_, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_WB_Set_, 1 * LV_HOR_RES, 0 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_WB_Set_);
  WB_Set_Page(Page_WB_Set_);

  Page_Setting = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
  lv_obj_set_size(Page_Setting, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_Setting, 2 * LV_HOR_RES, 1 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_Setting);

  lv_obj_t *Setting_Text = lv_label_create(Page_Setting, NULL);
  lv_obj_add_style(Setting_Text, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
  lv_label_set_text(Setting_Text, "BLE/WIFI!");
  lv_obj_align(Setting_Text, Page_Setting, LV_ALIGN_IN_TOP_MID, 0, 120);

  Page_SDUSB = lv_obj_create(tileview, Page_weather);
  lv_obj_set_size(Page_SDUSB, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(Page_SDUSB, 1 * LV_HOR_RES, 2 * LV_VER_RES);
  lv_tileview_add_element(tileview, Page_SDUSB);
  lv_obj_t *SDUSB = lv_label_create(Page_SDUSB, NULL);
  lv_obj_add_style(SDUSB, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
  lv_label_set_text(SDUSB, "SD/USB!");
  lv_obj_align(SDUSB, Page_SDUSB, LV_ALIGN_IN_TOP_MID, 0, 120);
}
