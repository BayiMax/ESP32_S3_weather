/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-27 00:44:58
 * @FilePath: \esps3_V1\components\SHT20\include\sht20.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef SHT20_H
#define SHT20_H

#include "stdio.h"

typedef struct
{
  float SHT_T;
  float SHT_H;
  volatile bool SHT_Flag;
} _SHT_TYP;
extern _SHT_TYP SHT_Value;
void SHT20_Test(void *pvParameters);

#endif /* SHT20_H */
