#include "Watch.h"

lv_obj_t *WIFI_BTN_TEXT;
lv_obj_t *WIFI_BTN;
lv_obj_t *BLE_BTN_TEXT;
lv_obj_t *BLE_BTN;
lv_obj_t *BLK_Set_text;

_OUTSWITCH Out_Switch;

SemaphoreHandle_t BinarySem_OutSwitch = NULL;
volatile char Button_Wifi_Flag = trigger_attonity;
volatile char BUtton_Ble_Flag = 0;

/**
 * @brief
 * @param obj
 * @param event
 */
static void Wifi_Btn_Event(lv_obj_t *obj, lv_event_t event)
{
  BaseType_t xReturn = pdPASS;
  static char flag;
  if (event == LV_EVENT_CLICKED)
  {
    if (flag == 0)
    {
      flag = 1;
      lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x22165c));
      Button_Wifi_Flag = trigger_close;
    }
    else if (flag == 1)
    {
      flag = 0;
      lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));
      Button_Wifi_Flag = trigger_open;
    }
    xReturn = xSemaphoreGive(BinarySem_OutSwitch);
    if (xReturn == pdTRUE)
    {
      MY_DEBUG("wifi释放信号量成功");
    }
  }
  else if (event == LV_EVENT_LONG_PRESSED)
  {
    MY_DEBUG("长按");
  }
}
/**
 * @brief
 *
 * @param obj
 * @param event
 */
static void BLE_Btn_Event(lv_obj_t *obj, lv_event_t event)
{
  BaseType_t xReturn = pdPASS;
  static char flag;
  if (event == LV_EVENT_CLICKED)
  {
  }
  else if (event == LV_EVENT_VALUE_CHANGED)
  {
    if (flag == 0)
    {
      flag = 1;
      lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x22165c));
      BUtton_Ble_Flag = 1;
    }
    else if (flag == 1)
    {
      flag = 0;
      lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8));
    }
    xReturn = xSemaphoreGive(BinarySem_OutSwitch);
    if (xReturn == pdTRUE)
    {
      MY_DEBUG("BLE释放信号量成功");
    }
  }
}
/**
 * @brief
 * @param obj
 * @param event
 */
static void BLKSet_Btn_Event(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
    snprintf(buf, 4, "%u", lv_slider_get_value(obj));
    lv_label_set_text(BLK_Set_text, buf);
  }
}

/**
 * @brief outswitch page
 *
 * @param page
 */
void WB_Set_Page(lv_obj_t *page)
{
  /*信号量创建*/
  // taskENTER_CRITICAL(); // 进入临界区
  BinarySem_OutSwitch = xSemaphoreCreateBinary();
  if (NULL != BinarySem_OutSwitch)
  {
    MY_DEBUG("信号量创建成功！");
  }
  xTaskCreate(&OutSwitch_tast, "OutSwitch_tast", 1024 * 2, NULL, Priority_OutSwitch, NULL);
  // taskEXIT_CRITICAL(); // 退出临界区
  /**/
  static lv_style_t Font42;
  lv_style_init(&Font42); // 初始化样式
  lv_style_set_text_font(&Font42,
                         LV_STATE_DEFAULT, &lv_font_montserrat_44); // 设置字体
  lv_style_set_text_color(&Font42,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  static lv_style_t Font16;
  lv_style_init(&Font16); // 初始化样式
  lv_style_set_text_font(&Font16,
                         LV_STATE_DEFAULT, &lv_font_montserrat_16); // 设置字体
  lv_style_set_text_color(&Font16,
                          LV_STATE_DEFAULT, lv_color_hex(0xffffff)); // 设置字体颜色

  static lv_style_t BUTTON_Style;
  lv_style_init(&BUTTON_Style);
  lv_style_set_radius(&BUTTON_Style, LV_STATE_DEFAULT, 30);                       // 圆角
  lv_style_set_bg_opa(&BUTTON_Style, LV_STATE_DEFAULT, LV_OPA_COVER);             // 背景透明度
  lv_style_set_bg_color(&BUTTON_Style, LV_STATE_DEFAULT, lv_color_hex(0x3e28a8)); // 背景颜色
  // lv_style_set_bg_grad_color(&BUTTON_Style, LV_STATE_DEFAULT, LV_COLOR_RED);// 背景渐变的颜色
  // lv_style_set_bg_grad_dir(&BUTTON_Style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);//渐变到某颜色

  /*Swap the colors in pressed state*/
  // lv_style_set_bg_color(&BUTTON_Style, LV_STATE_PRESSED, lv_color_hex(0x3e28a8));
  // lv_style_set_bg_grad_color(&BUTTON_Style, LV_STATE_PRESSED, LV_COLOR_SILVER);

  lv_style_set_border_width(&BUTTON_Style, LV_STATE_DEFAULT, 0); // 无边框

  /*Set the text style*/
  // lv_style_set_text_color(&BUTTON_Style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  WIFI_BTN = lv_btn_create(page, NULL);
  lv_obj_set_event_cb(WIFI_BTN, Wifi_Btn_Event); // 添加事件
  lv_obj_set_size(WIFI_BTN, 60, 60);             // 大小
  lv_obj_align(WIFI_BTN, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
  lv_obj_reset_style_list(WIFI_BTN, LV_BTN_PART_MAIN);
  lv_obj_add_style(WIFI_BTN, LV_BTN_PART_MAIN, &BUTTON_Style);
  lv_btn_set_checkable(WIFI_BTN, true);
  lv_btn_toggle(WIFI_BTN);
  // lv_btn_set_fit2(WIFI_BTN, LV_FIT_NONE, LV_FIT_TIGHT);//自适应大小

  WIFI_BTN_TEXT = lv_label_create(WIFI_BTN, NULL);
  lv_obj_add_style(WIFI_BTN_TEXT, LV_LABEL_PART_MAIN, &Font42); // 为对象添加样式
  lv_label_set_text(WIFI_BTN_TEXT, LV_SYMBOL_WIFI);

  BLE_BTN = lv_btn_create(page, NULL);
  lv_obj_set_event_cb(BLE_BTN, BLE_Btn_Event); // 添加事件
  lv_obj_set_size(BLE_BTN, 60, 60);            // 大小
  lv_obj_align(BLE_BTN, WIFI_BTN, LV_ALIGN_OUT_LEFT_MID, -15, 0);
  lv_obj_reset_style_list(BLE_BTN, LV_BTN_PART_MAIN);
  lv_obj_add_style(BLE_BTN, LV_BTN_PART_MAIN, &BUTTON_Style);
  lv_btn_set_checkable(BLE_BTN, true);
  lv_btn_toggle(BLE_BTN);
  // lv_btn_set_fit2(BLE_BTN, LV_FIT_NONE, LV_FIT_TIGHT);//自适应大小

  BLE_BTN_TEXT = lv_label_create(BLE_BTN, NULL);
  lv_obj_add_style(BLE_BTN_TEXT, LV_LABEL_PART_MAIN, &Font42); // 为对象添加样式
  lv_label_set_text(BLE_BTN_TEXT, LV_SYMBOL_BLUETOOTH);

  lv_obj_t *BLK_Set = lv_slider_create(page, NULL);
  lv_obj_set_width(BLK_Set, 200);
  lv_obj_set_height(BLK_Set, 10);
  lv_obj_align(BLK_Set, NULL, LV_ALIGN_IN_TOP_MID, 0, 240);
  lv_obj_set_event_cb(BLK_Set, BLKSet_Btn_Event);
  lv_slider_set_range(BLK_Set, 0, 255);
  lv_slider_set_value(BLK_Set, 255, LV_ANIM_ON);
  BLK_Set_text = lv_label_create(page, NULL);
  lv_obj_add_style(BLK_Set_text, LV_LABEL_PART_MAIN, &Font16); // 为对象添加样式
  lv_label_set_text(BLK_Set_text, "255");
  lv_obj_set_auto_realign(BLK_Set_text, true);
  lv_obj_align(BLK_Set_text, BLK_Set, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void OutSwitch_tast(void *pvParameters)
{
  BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */
  while (1)
  {
    /* 二值信号量句柄
    xSemaphoreTake获取一个信号量,可以是二值信号量、计数信号量、互斥量。*/
    xReturn = xSemaphoreTake(BinarySem_OutSwitch, portMAX_DELAY);
    if (xReturn == pdTRUE)
    {
      MY_DEBUG("获取信号量成功");
      if (Button_Wifi_Flag == trigger_open)
      {
        Button_Wifi_Flag = trigger_attonity;
        if (U_wifi_data.State == Wifi_connection_break)
        {
          MY_DEBUG("连接wifi");
          wifi_connect_ap(0, U_wifi_data.SSID, U_wifi_data.PASS, NULL);
        }
      }
      else if (Button_Wifi_Flag == trigger_close)
      {
        Button_Wifi_Flag = trigger_attonity;
        if (U_wifi_data.State == Wifi_connected)
        {
          MY_DEBUG("关闭wifi");
          Wifi_connect_break();
        }
      }

      if (BUtton_Ble_Flag == 1)
      {
        BUtton_Ble_Flag = 0;
        wifi_scanList_start();
      }
    }
  }
}