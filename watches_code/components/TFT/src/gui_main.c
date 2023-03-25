/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-31 14:32:39
 * @FilePath: \esps3_V1\components\TFT\src\gui_main.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "lvgl.h"
#include "lvgl_helpers.h"
#include "lv_fs_if.h"

#include "gui_main.h"
#include "Watch.h"
#include "User_Touch.h"
#include "lvgl_touch/touch_driver.h"
#define LV_TICK_PERIOD_MS 1

SemaphoreHandle_t xGuiSemaphore; // 创建信号量进行对lvgl的并发调用

static void create_demo_application(void);
static void lv_tick_task(void *arg);
/**
 * @brief LVGL任务函数
 *
 * @param pvParameter
 */
void gui_task(void *pvParameter)
{
  (void)pvParameter;
  xGuiSemaphore = xSemaphoreCreateMutex();
  /*lvgl系统内核初始化*/
  lv_init();
  /*lvgl文件系统初始化*/
  lv_fs_if_init();
  /*初始化PWM背光*/
  // Screen_Black_Light_Init();
  // Screen_Black_light(255);
  /* Initialize SPI or I2C bus used by the drivers */
  lvgl_driver_init();

  lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf1 != NULL);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
  lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf2 != NULL);
#else
  static lv_color_t *buf2 = NULL;
#endif
  static lv_disp_buf_t disp_buf;

  uint32_t size_in_px = DISP_BUF_SIZE;
  lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  disp_drv.flush_cb = disp_driver_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.read_cb = touch_driver_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);

  const esp_timer_create_args_t periodic_timer_args = {
      .callback = &lv_tick_task,
      .name = "periodic_gui"};
  esp_timer_handle_t periodic_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

  /* Create the demo application */

  /*旋转某个显示*/
  // lv_disp_set_rotation(NULL, LV_DISP_ROT_90);

  create_demo_application();

  while (1)
  {
    /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
    vTaskDelay(pdMS_TO_TICKS(10));

    // /* Try to take the semaphore, call lvgl related function on success */
    // if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    // {
    lv_task_handler();
    // xSemaphoreGive(xGuiSemaphore);
    // }
  }

  /* A task should NEVER return */
  free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
  free(buf2);
#endif
  vTaskDelete(NULL);
}
/**
 * @brief Create a demo application object
 *
 */
static void create_demo_application(void)
{
  lv_Watch_theme();
  // User_Gui_Test();
}
/**
 * @brief LVGL心跳
 *
 * @param arg
 */
static void lv_tick_task(void *arg)
{
  (void)arg;
  lv_tick_inc(LV_TICK_PERIOD_MS);
}

void Screen_Black_Light_Init(void)
{
  ledc_timer_config_t Black_t = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .duty_resolution = LEDC_TIMER_13_BIT,
      .freq_hz = Black_HZ, // Set output frequency at 5 kHz
      .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&Black_t));

  ledc_channel_config_t Black_Out_t = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .timer_sel = LEDC_TIMER_0,
      .intr_type = LEDC_INTR_DISABLE,
      .gpio_num = LEDC_OUTPUT_IO,
      .duty = 0, // Set duty to 0%
      .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&Black_Out_t));
}
/**
 * @brief
 * @param light 亮度调节0`255
 */
void Screen_Black_light(uint8_t light)
{
  /*设置PWM*/
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (light * 32)));
  /*更新应用输出*/
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}
