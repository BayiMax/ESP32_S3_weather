/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-06 09:02:04
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-19 21:16:47
 * @FilePath: \esps3_V1\components\User_I2C\src\User_I2C.c
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

#define I2C_SCL_NUM 7
#define I2C_SDA_NUM 6

/**
 * @brief i2c 主 初始化
 */
esp_err_t I2C_slave_init(void)
{
  int i2c_master_port = I2C_SLAVE_PORT_NUM;
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SDA_NUM,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_SCL_NUM,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
      // .clk_flags = 0,
      /*!<用于选择I2C时钟*/
  };
  esp_err_t err = i2c_param_config(i2c_master_port, &conf);
  if (err != ESP_OK)
  {
    return err;
  }
  return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
/**
 * @brief
 *
 * @param i2c_num
 * @param addr
 * @param reg
 * @param buf
 * @param len
 * @return esp_err_t
 */
esp_err_t I2C_Read_Len(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  /*
    i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret != ESP_OK)
  {
    MY_DEBUG("error!");
    return ret;
  }
  vTaskDelay(80 / portTICK_RATE_MS);
  cmd = i2c_cmd_link_create();*/

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
  while (len)
  {
    if (len == 1)
    {
      i2c_master_read_byte(cmd, buf, NACK_VAL);
    }
    else
    {
      i2c_master_read_byte(cmd, buf, ACK_VAL);
    }
    len--;
    buf++;
  }
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%x.", ret);
  }
  return ret;
}
/**
 * @brief
 *
 * @param i2c_num
 * @param addr
 * @param reg
 * @param buf
 * @param len
 * @return esp_err_t
 */
esp_err_t I2C_Write_Len(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
  uint8_t i;
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  for (i = 0; i < len; i++)
  {
    i2c_master_write_byte(cmd, buf[i], ACK_CHECK_EN);
  }
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
  return ret;
}
/**
 * @brief
 *
 * @param i2c_num
 * @param addr
 * @param reg
 * @param data
 * @return esp_err_t
 */
esp_err_t I2C_Write_byte(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t data)
{
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
  return ret;
}
/**
 * @brief
 *
 * @param i2c_num
 * @param addr
 * @param reg
 * @param data
 * @return esp_err_t
 */
esp_err_t I2C_Read_byte(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *data)
{
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, data, NACK_VAL);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
  return ret;
}