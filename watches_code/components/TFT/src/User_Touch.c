#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "User_Touch.h"
#include "User_I2C.h"
#include "main.h"

_USER_TOUCH U_Touch;
uint8_t Pen_Flag;
/* 设备地址 */
#define Touch_Write_Addr 0x2A /*写地址*/
#define Touch_Read_Addr 0x2B  /*读地址*/
/**
 * @brief
 *
 * @param reg
 * @param data
 * @return esp_err_t
 */
esp_err_t Touch_Write_Byte(uint8_t reg, uint8_t data)
{
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Touch_Write_Addr, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(TOUCH_SLAVE_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
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
 * @param reg
 * @param data
 * @return esp_err_t
 */
esp_err_t Touch_Read_Byte(uint8_t reg, uint8_t *data)
{
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Touch_Write_Addr, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Touch_Read_Addr, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, data, NACK_VAL);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(TOUCH_SLAVE_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
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
 * @param ID ID寄存器
 */
esp_err_t Get_Touch_ID(unsigned char *ID)
{
  int ret = 0;
  ret = Touch_Read_Byte(ChipID, ID);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
  return ret;
}
/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t Touch_Get_XYPos(void)
{
  int ret = 0;
  uint8_t Dat[4];
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Touch_Write_Addr, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, XposH, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Touch_Read_Addr, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, &Dat[0], ACK_VAL);
  i2c_master_read_byte(cmd, &Dat[1], ACK_VAL);
  i2c_master_read_byte(cmd, &Dat[2], ACK_VAL);
  i2c_master_read_byte(cmd, &Dat[3], NACK_VAL);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(TOUCH_SLAVE_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
  U_Touch.XPos = ((Dat[0] & 0x0f) << 8) | Dat[1];
  U_Touch.YPos = ((Dat[2] & 0x0f) << 8) | Dat[3];
  return ret;
}

void Touch_Get_points_num(unsigned char *touch_pos_num)
{
  int ret = 0;
  ret = Touch_Read_Byte(0x02, touch_pos_num);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}

/**
 * @brief 使能连续动作(连续左右滑动,连续上下滑动,双击)
 *使能连续动作会增加响应时间
 * @param mode 5
 */
void Touch_Config_MotionMask(uint8_t mode)
{
  int ret = 0;
  ret = Touch_Write_Byte(MotionMask, mode);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 规定time内无触摸,自动进入低功耗模式
 *
 * @param time 时间
 */
void Touch_Config_AutoSleepTime(uint8_t time)
{
  int ret = 0;
  ret = Touch_Write_Byte(AutoSleepTime, time);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 手势检测滑动分区角度控制.Angle=tan(c)*10 c:为以x轴正方向为基准的角度
 *
 * @param angle 角度
 */
void Touch_Config_MotionSlAngle(uint8_t angle)
{
  int ret = 0;
  ret = Touch_Write_Byte(MotionSlAngle, angle);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 正常快速检测周期配置函数
 *
 * @param Period 周期值 1~30
 */
void Touch_Config_NorScanPer(uint8_t Period)
{
  int ret = 0;
  if (Period >= 30)
  {
    Period = 30;
  }
  ret = Touch_Write_Byte(NorScanPer, Period);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 中断低脉冲输出宽度配置函数
 *
 * @param Width 宽度(周期值)
 */
void Touch_Config_IrqPluseWidth(uint8_t Width)
{
  int ret = 0;
  if (Width >= 200)
  {
    Width = 200;
  }
  ret = Touch_Write_Byte(IrqPluseWidth, Width);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 低功耗扫描唤醒门限配置函数.越小越灵敏.默认值48
 *
 * @param TH 门限值
 */
void Touch_Config_LpScanTH(uint8_t TH)
{
  int ret = 0;
  ret = Touch_Write_Byte(LpScanTH, TH);
  if (ret == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("error!ret:%d.", ret);
  }
}
/**
 * @brief 触摸初始化
 *
 */
void User_Touch_Init(void)
{
  esp_err_t err = 0;
  int touch_master_port = TOUCH_SLAVE_PORT_NUM;
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = TOUCH_SDA_NUM,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = TOUCH_SCL_NUM,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = TOUCH_MASTER_FREQ_HZ,
      // .clk_flags = 0,
      /*!<用于选择I2C时钟*/
  };
  err = i2c_param_config(touch_master_port, &conf);
  if (err != ESP_OK)
  {
    MY_DEBUG("ERROR%d.", err);
  }
  err = i2c_driver_install(touch_master_port, conf.mode, TOUCH_MASTER_RX_BUF_DISABLE, TOUCH_MASTER_TX_BUF_DISABLE, 0);
  if (err != ESP_OK)
  {
    MY_DEBUG("ERROR%d.", err);
  }
  err = Get_Touch_ID(&U_Touch.ID);
  if (err == ESP_ERR_TIMEOUT)
  {
    MY_DEBUG("ERROR%d.", err);
  }
}
