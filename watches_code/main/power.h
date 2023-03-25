/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-18 14:48:37
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 13:28:37
 * @FilePath: \esps3_V1\main\power.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef POWER_H
#define POWER_H

#include "stdio.h"

#define BAT_FULL 2.121f
#define BAT_EMPTY 1.7f

typedef struct
{
  void (*OTHER)(unsigned char mode);
  void (*SW)(unsigned char mode);
  volatile int Key;
  double BAT_Value;
  volatile char Run_Flag;
  volatile char OTHER_Flag;
} _POWER_CONTROL;

extern _POWER_CONTROL Power_C;
void Power_Init(void);
void Power_Test(void *pvParameters);

#endif /* POWER_H */
