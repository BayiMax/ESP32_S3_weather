/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-08-30 20:21:17
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-03 01:14:01
 * @FilePath: \esps3_V1\main\main.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MAIN_H
#define _MAIN_H

#include "stdio.h"
#include "math.h"
#include "string.h"
/*任务句柄*/
extern TaskHandle_t Weather_test_Handle;
extern TaskHandle_t Time_test_Handle;
extern TaskHandle_t Web_Test_Handle;
extern TaskHandle_t SHT20_Test_Handle;
extern TaskHandle_t MPU_Test_Handle;
extern TaskHandle_t Power_Test_Handle;

/*任务优先级*/
#define Priority_Weather 16
#define Priority_Time 15
#define Priority_Web 14
#define Priority_SHT20 18
#define Priority_MUP 17
#define Priority_Power 4
#define Priority_LVGL 1
#define Priority_OutSwitch 7

/**/

#define MY_DEBUG(format, ...) printf("<B:%s:%d-%s>:" format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif /*_MAIN_H*/