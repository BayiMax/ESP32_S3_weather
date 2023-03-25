/*
 * PROJECT:   LVGL ported to Windows Desktop
 * FILE:      LVGL.Windows.Desktop.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "resource.h"

#if _MSC_VER >= 1200
 // Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lv_examples/lv_examples.h"
#include "win32drv_v7.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif
#include <stdio.h>
/*USER start*/

#define U_BACK_COLOR lv_color_hex(0x140d36)

lv_style_t weather_22Font; // 创建22样式
lv_style_t weather_16Font; // 创建16样式
lv_style_t Time_42Font;
lv_obj_t* location_txt;

lv_obj_t* timeShow;
lv_obj_t* dateShow;
lv_obj_t* textdayShow;

static lv_obj_t* bg_top;
/**
 * @brief 顶部显示框
 *
 * @param Top 样式
 */
static void This_Top(lv_obj_t* Top)
{
    lv_obj_t* Down_btn = lv_label_create(Top, NULL);
    lv_obj_add_style(Down_btn, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
    lv_label_set_text(Down_btn, LV_SYMBOL_DOWN);
    lv_obj_align(Down_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    lv_obj_t* wifi;
    wifi = lv_label_create(Top, NULL);
    lv_obj_add_style(wifi, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
    lv_label_set_text(wifi, LV_SYMBOL_WIFI);
    lv_obj_align(wifi, Down_btn, LV_ALIGN_OUT_LEFT_MID, -60, -5);

    lv_obj_t* BLE;
    BLE = lv_label_create(Top, NULL);
    lv_obj_add_style(BLE, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
    lv_label_set_text(BLE,LV_SYMBOL_BLUETOOTH);
    lv_obj_align(BLE, wifi, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    lv_obj_t* POWER;
    POWER = lv_label_create(Top, NULL);
    lv_obj_add_style(POWER, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
    lv_label_set_text(POWER, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(POWER, Down_btn, LV_ALIGN_OUT_RIGHT_MID, 60, -5);
}
/**
 * @brief 次顶部显示框
 * 时间,日期,定位
 *
 * @param Top_two 样式
 */
static void This_Toptwo(lv_obj_t* Top_two)
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

    lv_obj_t* location_icon=lv_label_create(Top_two, NULL);
    lv_obj_add_style(location_icon, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式

    lv_obj_set_style_local_text_color(location_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT,lv_color_hex(0xc0c0c0));

    lv_label_set_text(location_icon, LV_SYMBOL_GPS);
    lv_obj_align(location_icon, location_txt, LV_ALIGN_OUT_LEFT_MID, -8, 0);
}

/**
 * @brief 天气页面主显示框
 *
 * @param show_in 样式
 */
static void This_show_in(lv_obj_t* show_in)
{
    lv_obj_t* temperature;
    lv_obj_t* text_day;
    lv_obj_t* text_night;
    lv_obj_t* rainfall;
    lv_obj_t* humidity;
    lv_obj_t* wind_direction;
    lv_obj_t* wind_direction_degree;
    lv_obj_t* wind_speed;
    lv_obj_t* wind_scale;

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
    lv_label_set_text(humidity, "70%");
    lv_obj_align(humidity, text_night, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    wind_direction = lv_label_create(show_in, NULL);
    lv_obj_add_style(wind_direction, LV_LABEL_PART_MAIN, &weather_22Font);
    lv_label_set_text(wind_direction, "NW");
    lv_obj_align(wind_direction, rainfall, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    wind_direction_degree = lv_label_create(show_in, NULL);
    lv_obj_add_style(wind_direction_degree, LV_LABEL_PART_MAIN, &weather_22Font);
    lv_label_set_text(wind_direction_degree, "315'");
    lv_obj_align(wind_direction_degree, humidity, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    wind_speed = lv_label_create(show_in, NULL);
    lv_obj_add_style(wind_speed, LV_LABEL_PART_MAIN, &weather_22Font);
    lv_label_set_text(wind_speed, "8.4km/h");
    lv_obj_align(wind_speed, wind_direction, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    wind_scale = lv_label_create(show_in, NULL);
    lv_obj_add_style(wind_scale, LV_LABEL_PART_MAIN, &weather_22Font);
    lv_label_set_text(wind_scale, "2");
    lv_obj_align(wind_scale, wind_direction_degree, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);
}

/**
 * @brief 
 * @param obj 
*/
void User_Time_Page(lv_obj_t* obj)
{
    lv_obj_t* time_value;
    time_value = lv_label_create(obj, NULL);
    lv_obj_add_style(time_value, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(time_value, "55:55:55");
    lv_obj_align(time_value, NULL, LV_ALIGN_IN_TOP_MID, 0, 140);
    lv_obj_t* data_value;
    data_value = lv_label_create(obj, NULL);
    lv_obj_add_style(data_value, LV_LABEL_PART_MAIN, &weather_22Font); // 为对象添加样式
    lv_label_set_text(data_value, "2022-10-31");
    lv_obj_align(data_value, time_value, LV_ALIGN_OUT_BOTTOM_MID, 20, 0);
}

/**
 * @author BBX
 * @date 2022年9月27日10点41分
 * @brief 天气界面
 * @param  main_obj 主
 * @return NULL
 */
void Weather_page(lv_obj_t* page)
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

    lv_style_init(&Time_42Font); // 初始化样式
    lv_style_set_text_font(&Time_42Font,
        LV_STATE_DEFAULT, &lv_font_montserrat_42); // 设置字体
    lv_style_set_text_color(&Time_42Font,
        LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

    This_Top(page);
    This_Toptwo(page);
    This_show_in(page);
}
void Lock_Page(lv_obj_t* page) {
    lv_obj_t* this_lock_text = lv_label_create(page, NULL);
    lv_obj_add_style(this_lock_text, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(this_lock_text, LV_SYMBOL_UP);
    lv_obj_align(this_lock_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 120);
}

lv_obj_t* WIFI_BTN_TEXT;
lv_obj_t* WIFI_BTN;
lv_obj_t* BLE_BTN_TEXT;
lv_obj_t* BLE_BTN;
lv_obj_t* BLK_Set_text;


/**
 * @brief 
 * @param obj 
 * @param event 
*/
static void Wifi_Btn_Event(lv_obj_t * obj, lv_event_t event)
{
    static char flag = 0;

    if (event == LV_EVENT_VALUE_CHANGED) {
        if (flag == 0) {
            flag = 1;
            lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x22165c));
        }
        else if (flag == 1) {
            flag = 0;
            lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));
        }
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
        
    }
}
/**
 * @brief 
 * @param obj 
 * @param event 
*/
static void BLE_Btn_Event(lv_obj_t* obj, lv_event_t event) {
    static char flag = 0;
    if (event == LV_EVENT_CLICKED) {
    }
    else if (event == LV_EVENT_VALUE_CHANGED) {
        if (flag == 0) {
            flag = 1;
            lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x22165c));
        }
        else if (flag == 1) {
            flag = 0;
            lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));
        }
    }
}
/**
 * @brief 
 * @param obj 
 * @param event 
*/
static void BLKSet_Btn_Event(lv_obj_t* obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
        snprintf(buf, 4, "%u", lv_slider_get_value(obj));
        lv_label_set_text(BLK_Set_text, buf);
    }
}

void WB_Set_Page(lv_obj_t* page) {
    static lv_style_t BUTTON_Style;
    static lv_style_t SLIDER_Style;
    lv_style_init(&BUTTON_Style);
    lv_style_set_radius(&BUTTON_Style, LV_STATE_DEFAULT, 30);//圆角
    lv_style_set_bg_opa(&BUTTON_Style, LV_STATE_DEFAULT, LV_OPA_COVER);//背景透明度
    lv_style_set_bg_color(&BUTTON_Style, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));//背景颜色
    lv_style_set_border_width(&BUTTON_Style, LV_STATE_DEFAULT, 0);//无边框
    //lv_style_set_bg_grad_color(&BUTTON_Style, LV_STATE_DEFAULT, LV_COLOR_RED);// 背景渐变的颜色
    //lv_style_set_bg_grad_dir(&BUTTON_Style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);//渐变到某颜色
    /*Swap the colors in pressed state*/
    //lv_style_set_bg_color(&BUTTON_Style, LV_STATE_PRESSED, lv_color_hex(0x3e28a8));
    //lv_style_set_bg_grad_color(&BUTTON_Style, LV_STATE_PRESSED, LV_COLOR_SILVER);
    /*Set the text style*/
    //lv_style_set_text_color(&BUTTON_Style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_style_init(&SLIDER_Style);
    lv_style_set_bg_opa(&SLIDER_Style, LV_STATE_DEFAULT, LV_OPA_100);//背景透明度
    lv_style_set_bg_color(&SLIDER_Style, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));//背景颜色
    lv_style_set_border_width(&SLIDER_Style, LV_STATE_DEFAULT, 0);//无边框

    WIFI_BTN = lv_btn_create(page, NULL);
    lv_obj_set_event_cb(WIFI_BTN, Wifi_Btn_Event);//添加事件
    lv_obj_set_size(WIFI_BTN,60,60);//大小
    lv_obj_align(WIFI_BTN, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
    lv_obj_reset_style_list(WIFI_BTN, LV_BTN_PART_MAIN);
    lv_obj_add_style(WIFI_BTN, LV_BTN_PART_MAIN, &BUTTON_Style);
    lv_btn_set_checkable(WIFI_BTN, true);
    lv_btn_toggle(WIFI_BTN);
    //lv_btn_set_fit2(WIFI_BTN, LV_FIT_NONE, LV_FIT_TIGHT);//自适应大小

    WIFI_BTN_TEXT = lv_label_create(WIFI_BTN, NULL);
    lv_obj_add_style(WIFI_BTN_TEXT, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(WIFI_BTN_TEXT, LV_SYMBOL_WIFI);

    BLE_BTN = lv_btn_create(page, NULL);
    lv_obj_set_event_cb(BLE_BTN, BLE_Btn_Event);//添加事件
    lv_obj_set_size(BLE_BTN, 60, 60);//大小
    lv_obj_align(BLE_BTN, WIFI_BTN, LV_ALIGN_OUT_LEFT_MID, -15, 0);
    lv_obj_reset_style_list(BLE_BTN, LV_BTN_PART_MAIN);
    lv_obj_add_style(BLE_BTN, LV_BTN_PART_MAIN, &BUTTON_Style);
    lv_btn_set_checkable(BLE_BTN, true);
    lv_btn_toggle(BLE_BTN);
    //lv_btn_set_fit2(BLE_BTN, LV_FIT_NONE, LV_FIT_TIGHT);//自适应大小

    BLE_BTN_TEXT = lv_label_create(BLE_BTN, NULL);
    lv_obj_add_style(BLE_BTN_TEXT, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(BLE_BTN_TEXT, LV_SYMBOL_BLUETOOTH);

    lv_obj_t* BLK_Set = lv_slider_create(page,NULL);
    lv_obj_set_width(BLK_Set, 200);
    lv_obj_set_height(BLK_Set,10);
    lv_obj_add_style(BLK_Set, LV_BTN_PART_MAIN, &SLIDER_Style);
    lv_obj_align(BLK_Set, NULL, LV_ALIGN_IN_TOP_MID, 0, 200);
    lv_obj_set_event_cb(BLK_Set, BLKSet_Btn_Event);
    lv_slider_set_range(BLK_Set, 0, 255);
    lv_slider_set_value(BLK_Set,255,LV_ANIM_ON);
    BLK_Set_text = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(BLK_Set_text, LV_LABEL_PART_MAIN, &weather_16Font); // 为对象添加样式
    lv_label_set_text(BLK_Set_text, "255");
    lv_obj_set_auto_realign(BLK_Set_text, true);
    lv_obj_align(BLK_Set_text, BLK_Set, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}


/**
 * @author Bay_Max
 * @date 2022年9月27日10点42分
 * @brief 创建wether界面主题
 * @param 无
 * @return 无
 */
void lv_Weather_theme(void)
{
    lv_obj_t* Page_weather;
    lv_obj_t* Page_time;
    lv_obj_t* Page_WB_Set_;
    lv_obj_t* Page_Setting;
    lv_obj_t* Page_SDUSB;
    static char User_Page_num = 6;
    static lv_point_t Valid_pos[] = { {0, 0}, {0, 1}, {1, 1}, {1,0},{2,1},{1,2} };

    lv_obj_t* scr = lv_obj_create(NULL, NULL); // 创建一个基本对象
    lv_scr_load(scr);                          // 激活屏幕

    lv_obj_t* tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, Valid_pos, User_Page_num);
    lv_tileview_set_edge_flash(tileview, true);              // 启用边缘闪光效果
    lv_page_set_scrlbar_mode(tileview, LV_SCRLBAR_MODE_OFF); // 取消滚动条

    Page_weather = lv_obj_create(tileview, NULL);            // 创建一个基本对象
    lv_obj_clean_style_list(Page_weather, LV_OBJ_PART_MAIN); // 将样式重置为空
    lv_obj_set_style_local_radius(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,46);
    lv_obj_set_style_local_bg_opa(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(Page_weather, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, U_BACK_COLOR);
    lv_obj_set_size(Page_weather, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_weather, 1* LV_HOR_RES, 1* LV_VER_RES);
    lv_tileview_add_element(tileview, Page_weather);
    Weather_page(Page_weather);

    Page_time = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
    lv_obj_set_size(Page_time, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_time, 0*LV_HOR_RES, 1* LV_VER_RES);
    lv_tileview_add_element(tileview, Page_time);
    User_Time_Page(Page_time);

    lv_obj_t* Page_lcok = lv_obj_create(tileview, Page_weather);
    lv_obj_set_size(Page_lcok, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_lcok, 0 * LV_HOR_RES, 0 * LV_VER_RES);
    lv_tileview_add_element(tileview, Page_lcok);
    Lock_Page(Page_lcok);

    Page_WB_Set_ = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
    lv_obj_set_size(Page_WB_Set_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_WB_Set_, 1 * LV_HOR_RES, 0 * LV_VER_RES);
    lv_tileview_add_element(tileview, Page_WB_Set_);
    WB_Set_Page(Page_WB_Set_);

    Page_Setting = lv_obj_create(tileview, Page_weather); // 创建一个基本对象
    lv_obj_set_size(Page_Setting, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_Setting, 2 * LV_HOR_RES, 1 * LV_VER_RES);
    lv_tileview_add_element(tileview, Page_Setting);

    lv_obj_t* Setting_Text = lv_label_create(Page_Setting, NULL);
    lv_obj_add_style(Setting_Text, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(Setting_Text, "BLE/WIFI!");
    lv_obj_align(Setting_Text, Page_Setting, LV_ALIGN_IN_TOP_MID, 0, 120);

    Page_SDUSB = lv_obj_create(tileview, Page_weather);
    lv_obj_set_size(Page_SDUSB, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(Page_SDUSB, 1 * LV_HOR_RES, 2 * LV_VER_RES);
    lv_tileview_add_element(tileview, Page_SDUSB);

    lv_obj_t* SDUSB = lv_label_create(Page_SDUSB, NULL);
    lv_obj_add_style(SDUSB, LV_LABEL_PART_MAIN, &Time_42Font); // 为对象添加样式
    lv_label_set_text(SDUSB, "SD/USB!");
    lv_obj_align(SDUSB, Page_SDUSB, LV_ALIGN_IN_TOP_MID, 0, 120);

}

/*USER end*/
int main()
{
    lv_init();
   
    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        240,
        280,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }

    /*
     * Demos, benchmarks, and tests.
     *
     * Uncomment any one (and only one) of the functions below to run that
     * item.
     */

    lv_Weather_theme();

    //lv_demo_widgets();
    //lv_demo_benchmark();
    //lv_demo_keypad_encoder();
    //lv_demo_printer();
    //lv_demo_stress();

    //lv_ex_get_started_1();      //计数按键
    //lv_ex_get_started_2();          //双按键切换
    //lv_ex_get_started_3();      //滑动条

    //lv_ex_style_1();
    //lv_ex_style_2();
    //lv_ex_style_3();
    //lv_ex_style_4();
    //lv_ex_style_5();
    //lv_ex_style_6();
    //lv_ex_style_7();
    //lv_ex_style_8();
    //lv_ex_style_9();
    //lv_ex_style_10();
    //lv_ex_style_11();

    /*
     * There are many examples of individual widgets found under the
     * lv_examples/src/lv_ex_widgets directory.  Here are a few sample test
     * functions.  Look in that directory to find all the rest.
     * 小部件例程
     */
     //lv_ex_arc_1();     //弧形进度条
     //lv_ex_cpicker_1();     //取色转盘
     //lv_ex_gauge_1();          //码表盘
     //lv_ex_img_1();       //图片
     //lv_ex_tileview_1(); //菜单

    while (!lv_win32_quit_signal)
    {
        lv_task_handler();
        Sleep(10);
    }

    return 0;
}
