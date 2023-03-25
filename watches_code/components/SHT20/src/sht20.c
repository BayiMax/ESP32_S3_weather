/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 *
 *
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *            佛祖保佑     永不宕机     永无BUG
 */

/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-06 09:02:04
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-18 21:25:18
 * @FilePath: \esps3_V1\components\SHT20\src\sht20.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "main.h"
#include "User_I2C.h"
#include "sht20.h"

_SHT_TYP SHT_Value;

#define SHT20_ADDR 0x40    /*SHT20地址*/
#define CRC_MODEL 0x131    /*CRC校验*/
#define Get_T 0xE3         /*保持主机 触发温度测量*/
#define Get_H 0xE5         /*保持主机 触发湿度测量*/
#define Get_N_T 0xF3       /*不保持主机 触发温度测量*/
#define Get_N_H 0xF5       /*不保持主机 触发湿度测量*/
#define W_Register 0xE6    /*写用户注册*/
#define R_Register 0xE7    /*读用户注册*/
#define Soft_reset 0xFE    /*复位*/
#define CRC_Error_R 9.999f /*CRC错误*/

/**
 * @brief SHT20_CRC
 *
 * @param ptr
 * @param len
 * @param checksum
 * @return true
 * @return false
 */
bool CRC_Check(unsigned char *ptr, unsigned char len, unsigned char checksum)
{
  unsigned char i;
  unsigned char crc = 0x00;

  while (len--)
  {
    crc ^= *ptr++;
    for (i = 8; i > 0; --i)
    {
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ CRC_MODEL;
      }
      else
        crc = (crc << 1);
    }
  }
  if (checksum == crc)
  {
    return true;
  }
  else
    return false;
}
/**
 * @brief Bay_Max:
 * 获取温湿度数据
 *
 * @param i2c_num
 * @param Addr
 * @param data
 * @param data_len
 * @param Cmd_RT
 * @return esp_err_t
 */
esp_err_t Get_SHT20_Value(
    i2c_port_t i2c_num, float *Rdata, uint8_t Cmd_RT)
{
  int ret = 0;
  uint8_t Get_data[2];
  uint8_t CRC_Value;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, 0x80, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, Cmd_RT, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret != ESP_OK)
  {
    MY_DEBUG("error!");
    return ret;
  }
  vTaskDelay(80 / portTICK_RATE_MS);
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (SHT20_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, &Get_data[0], ACK_VAL);
  i2c_master_read_byte(cmd, &Get_data[1], ACK_VAL);
  i2c_master_read_byte(cmd, &CRC_Value, NACK_VAL);

  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (CRC_Check(Get_data, 2, CRC_Value) == true)
  {
    *Rdata = ((Get_data[0] << 8) + Get_data[1]);
  }
  else
  {
    MY_DEBUG("%f,CRC", *Rdata);
    *Rdata = CRC_Error_R;
  }

  return ret;
}

/**
 * @brief SHT20任务
 *
 * @param pvParameters
 */
void SHT20_Test(void *pvParameters)
{
  int ret;
  float value;
  while (1)
  {
    /*获取温度*/
    ret = Get_SHT20_Value(I2C_SLAVE_PORT_NUM, &value, Get_N_T);
    if (ret == ESP_ERR_TIMEOUT)
    {
      MY_DEBUG("I2C Error in time out!");
    }
    else if (ret == ESP_OK)
    {
      if (value != CRC_Error_R)
      {
        SHT_Value.SHT_Flag = true;
        SHT_Value.SHT_T = (175.72 * value / 65536 - 46.85);
        // MY_DEBUG("SHT_T:%.2f.", SHT_Value.SHT_T);
      }
      else
      {
        MY_DEBUG("CRC_error!");
      }
    }
    /*获取湿度*/
    ret = Get_SHT20_Value(I2C_SLAVE_PORT_NUM, &value, Get_N_H);
    if (ret == ESP_ERR_TIMEOUT)
    {
      MY_DEBUG("I2C Error in time out!");
    }
    else if (ret == ESP_OK)
    {
      if (value != CRC_Error_R)
      {
        SHT_Value.SHT_Flag = true;
        SHT_Value.SHT_H = (125.0 * value / 65536 - 6.0);
        // MY_DEBUG("SHT_H:%.2f.", SHT_Value.SHT_H);
      }
      else
      {
        MY_DEBUG("CRC_error!");
      }
    }
    vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
  }
}
