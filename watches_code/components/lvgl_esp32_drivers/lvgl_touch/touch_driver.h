/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-24 23:52:27
 * @FilePath: \esps3_V1\components\lvgl_esp32_drivers\lvgl_touch\touch_driver.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
/**
 * @file touch_driver.h
 */

#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

  // #define USER_TOUCH_CST816T 1

#if defined(USER_TOUCH_CST816T)

#include "User_Touch.h"

#endif
#if defined(CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
#include "xpt2046.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT6X06)
#include "ft6x36.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_STMPE610)
#include "stmpe610.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_ADCRAW)
#include "adcraw.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT81X)
#include "FT81x.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_RA8875)
#include "ra8875_touch.h"
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_GT911)
// #include "gt911.h"
#include "User_Touch.h"
#endif

  /*********************
   *      DEFINES
   *********************/

  /**********************
   * GLOBAL PROTOTYPES
   **********************/

  void touch_driver_init(void);

#if LVGL_VERSION_MAJOR >= 8
  void touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
#else
bool touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOUCH_DRIVER_H */
