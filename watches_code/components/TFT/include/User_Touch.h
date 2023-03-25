/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-23 20:23:33
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-31 01:22:12
 * @FilePath: \esps3_V1\components\TFT\include\User_Touch.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef USER_TOUCH_H
#define USER_TOUCH_H

#include "stdio.h"
#include "driver/i2c.h"

#define TOUCH_SDA_NUM 35
#define TOUCH_SCL_NUM 34

#define TOUCH_SLAVE_PORT_NUM 1        /*选择使用I2C0*/
#define TOUCH_MASTER_FREQ_HZ 100000   /*I2C频率*/
#define TOUCH_MASTER_TX_BUF_DISABLE 0 /*!< 主机不需要缓冲区 */
#define TOUCH_MASTER_RX_BUF_DISABLE 0 /*!< 主机不需要缓冲区 */

/* 触摸屏寄存器 */
#define GestureID 0x01 /*手势识别*/
#define FingerNum 0x02 /**/
#define XposH 0x03     /*X高*/
#define XposL 0x04     /*X低*/
#define YposH 0x05     /*Y高*/
#define YposL 0x06     /*Y低*/
#define ChipID 0xA7    /*芯片ID*/
#define MotionMask 0xEC
#define IrqPluseWidth 0xED
#define NorScanPer 0xEE
#define MotionSlAngle 0xEF
#define LpAutoWakeTime 0xF4
#define LpScanTH 0xF5
#define LpScanWin 0xF6
#define LpScanFreq 0xF7
#define LpScanIdac 0xF8
#define AutoSleepTime 0xF9
#define IrqCtl 0xFA
#define AutoReset 0xFB
#define LongPressTime 0xFC
#define IOCtl 0xFD
#define DisAutoSleep 0xFE

typedef struct
{
  unsigned short XPos;
  unsigned short YPos;
  unsigned char Flag;
  unsigned char ID;
} _USER_TOUCH;

extern _USER_TOUCH U_Touch;
extern uint8_t Pen_Flag;
/* 手势ID识别选项 */
typedef enum
{
  NOGESTURE = 0x00,
  DOWNGLIDE = 0x01,
  UPGLIDE = 0x02,
  LEFTGLIDE = 0x03,
  RIGHTGLIDE = 0x04,
  CLICK = 0x05,
  DOUBLECLICK = 0x0B,
  LONGPRESS = 0x0C,
} GestureID_TypeDef;

/* 连续动作配置选项 */
typedef enum
{
  M_DISABLE = 0x00,
  EnConLR = 0x01,
  EnConUD = 0x02,
  EnDClick = 0x03,
  M_ALLENABLE = 0x07,
} MotionMask_TypeDef;

/* 中断低脉冲发射方式选项 */
typedef enum
{
  OnceWLP = 0x00,
  EnMotion = 0x10,
  EnChange = 0x20,
  EnTouch = 0x40,
  EnTest = 0x80,
} IrqCtl_TypeDef;

esp_err_t Touch_Write_Byte(uint8_t reg, uint8_t data);
esp_err_t Touch_Read_Byte(uint8_t reg, uint8_t *data);
esp_err_t Get_Touch_ID(unsigned char *ID);
esp_err_t Touch_Get_XYPos(void);
void Touch_Get_points_num(unsigned char *touch_pos_num);
void Touch_Config_MotionMask(uint8_t mode);
void Touch_Config_AutoSleepTime(uint8_t time);
void Touch_Config_MotionSlAngle(uint8_t angle);
void Touch_Config_NorScanPer(uint8_t Period);
void Touch_Config_IrqPluseWidth(uint8_t Width);
void Touch_Config_LpScanTH(uint8_t TH);
void User_Touch_Init(void);

#endif /* USER_TOUCH_H */
