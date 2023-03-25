/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-31 01:38:50
 * @FilePath: \esps3_V1\components\lvgl_esp32_drivers\lvgl_touch\touch_driver.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
/**
 * @file touch_driver.c
 */

#include "touch_driver.h"
#include "tp_spi.h"
#include "main.h"
void touch_driver_init(void)
{
#if defined(CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
    xpt2046_init();
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT6X06)
    ft6x06_init(FT6236_I2C_SLAVE_ADDR);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_STMPE610)
    stmpe610_init();
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_ADCRAW)
    adcraw_init();
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT81X)
    /* nothing to do */
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_RA8875)
    ra8875_touch_init();
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_GT911)
    // gt911_init(GT911_I2C_SLAVE_ADDR);
    User_Touch_Init();
#endif
#if defined(USER_TOUCH_CST816T)
    User_Touch_Init();
#endif
}
/**
 * @brief user read touch value
 *
 * @param drv
 * @param data
 * @return true
 * @return false
 */
bool Cst816t_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    esp_err_t ret;
    uint8_t touch_pnt_cnt = 0; // 检测到的触点数量
    // uint8_t status_reg;
    Touch_Get_points_num(&touch_pnt_cnt);
    ret = Touch_Get_XYPos();
    if (touch_pnt_cnt == 0)
    {
        data->point.x = 0;
        data->point.y = 0;
        data->state = LV_INDEV_STATE_REL;
        return false;
    }
    else
    {
        /*判断是否有超出屏幕范围的坐标返回*/
        data->point.x = U_Touch.XPos;
        data->point.y = U_Touch.YPos;
        data->state = LV_INDEV_STATE_PR;
    }

    // MY_DEBUG("x=%u y=%u.", data->point.x, data->point.y);
    // MY_DEBUG("X=%u Y=%u.", data->point.x, data->point.y);
    if (ret != ESP_OK)
    {
        MY_DEBUG("ERROR!%d.", ret);
    }
    return false;
}

#if LVGL_VERSION_MAJOR >= 8
void touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
#else
bool touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
#endif
{
    bool res = false;

#if defined(CONFIG_LV_TOUCH_CONTROLLER_XPT2046)
    res = xpt2046_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT6X06)
    res = ft6x36_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_STMPE610)
    res = stmpe610_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_ADCRAW)
    res = adcraw_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_FT81X)
    res = FT81x_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_RA8875)
    res = ra8875_touch_read(drv, data);
#elif defined(CONFIG_LV_TOUCH_CONTROLLER_GT911)
    // res = gt911_read(drv, data);
    res = Cst816t_read(drv, data);
#endif

#if defined(USER_TOUCH_CST816T)
    res = Cst816t_read(div, data);
#endif

#if LVGL_VERSION_MAJOR >= 8
    data->continue_reading = res;
#else
    return res;
#endif
}
