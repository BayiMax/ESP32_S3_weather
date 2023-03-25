/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-18 14:48:31
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 13:17:40
 * @FilePath: \esps3_V1\main\Power.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "power.h"
#include "esp_sleep.h"
#include "main.h"
_POWER_CONTROL Power_C;

#define OTHER_POWER_PIN 45 // 外设电源
#define POWER_SW_PIN 42    // 电池电源开关
#define POWER_IN_PIN 41    // 电池 按键 检测

/*ADC 宏定义*/
#define BAT_ADC_PIN ADC1_CHANNEL_4 // 电池电压检测
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
// ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11
static esp_adc_cal_characteristics_t adc1_chars; /*ADC初始化的结构体*/

#define GPIO_OUTPUT_PIN_SEL ((1ULL << OTHER_POWER_PIN) | (1ULL << POWER_SW_PIN))

void OTHER_POWER(unsigned char mode)
{
  gpio_set_level(OTHER_POWER_PIN, mode);
}
void POWER_SW(unsigned char mode)
{
  gpio_set_level(POWER_SW_PIN, mode);
}

void Power_Init(void)
{
  gpio_config_t io_conf = {};
  // disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  // set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  // disable pull-down mode
  io_conf.pull_down_en = 0;
  // disable pull-up mode
  io_conf.pull_up_en = 1;
  // configure GPIO with the given settings
  gpio_config(&io_conf);

  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.pin_bit_mask = POWER_IN_PIN;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = 0;
  io_conf.pull_down_en = 0;
  gpio_config(&io_conf);

  gpio_set_level(OTHER_POWER_PIN, 0);
  gpio_set_level(POWER_SW_PIN, 0);

  Power_C.OTHER = OTHER_POWER;
  Power_C.SW = POWER_SW;

  Power_C.SW(1);
}
static bool BAT_ADC_inits(void)
{
  esp_err_t ret;
  bool cali_enable = false;

  ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
  if (ret == ESP_ERR_NOT_SUPPORTED)
  {
    /*Calibration scheme not supported, skip software calibration*/
    MY_DEBUG("error!");
  }
  else if (ret == ESP_ERR_INVALID_VERSION)
  {
    /*eFuse not burnt, skip software calibration*/
    MY_DEBUG("error!");
  }
  else if (ret == ESP_OK)
  {
    cali_enable = true;
    /*描述ADC衰减特性*/
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
  }
  else
  {
    MY_DEBUG("Invalid arg");
  }
  return cali_enable;
}
void Power_Test(void *pvParameters)
{
  static int Old_Key_Value = 0;
  unsigned long voltage = 0;
  bool Cali_enable = BAT_ADC_inits();

  // ADC1-5 config
  /*配置ADC1捕获宽度*/
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
  ESP_ERROR_CHECK(adc1_config_channel_atten(BAT_ADC_PIN, ADC_EXAMPLE_ATTEN));
  while (1)
  {
    Power_C.Key = gpio_get_level(POWER_IN_PIN);

    if (Power_C.Key == 1)
    {
      Old_Key_Value = 1;
    }

    // MY_DEBUG("Key=%d.", Power_C.Key);
    for (char i = 0; i < 10; i++)
    {
      voltage += adc1_get_raw(BAT_ADC_PIN); // 读取ADC值
    }
    voltage = voltage / 10;
    voltage = voltage - 230;
    // MY_DEBUG("raw data: %ld.", voltage);
    if (Cali_enable)
    {
      Power_C.BAT_Value = (float)esp_adc_cal_raw_to_voltage(voltage, &adc1_chars) / 1000;
      // MY_DEBUG("cali data: %f mV.", Power_C.BAT_Value);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    Power_C.Key = gpio_get_level(POWER_IN_PIN);
    if (Old_Key_Value == 1)
    {
      if (Power_C.Key == 1)
      {
        Power_C.OTHER(0);
        esp_deep_sleep_start();
        // Power_C.SW(0);
      }
    }
  }
}
