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
 * @Date: 2022-08-12 18:49:36
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-06 09:01:15
 * @FilePath: \espc3_v1\main\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/* Hello World Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
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
#include "cJSON.h"
#include "esp_lcd_panel_io.h"

#include "main.h"
#include "User_Wifi.h"
#include "web_serve.h"
#include "Http_Weather.h"
#include "Http_time.h"

#include "watch.h"

#include "lvgl.h"
#include "lv_fs_if.h"
#include "gui_main.h"
#include "User_Touch.h"
#include "User_I2C.h"
#include "SHT20.h"
#include "mpu6050.h"
#include "power.h"
#include "web_serve.h"
#include "unity.h"
#include "mysd.h"

TaskHandle_t Weather_test_Handle;
TaskHandle_t Time_test_Handle;
TaskHandle_t SHT20_Test_Handle;
TaskHandle_t MPU_Test_Handle;
TaskHandle_t Power_Test_Handle;
TaskHandle_t Web_Test_Handle;

void app_main(void)
{
        uint8_t x = 0;
        int ress;
        static char InfoBuffer[500];
        printf("hello Bay Max\n");
        Power_Init();
        Power_C.Run_Flag = 0;
        Power_C.OTHER(1); /*使能外设电源*/
        I2C_slave_init();
        UMPU_Euler_Angle.Init_Flag = false;
        for (x = 0; x < 5; x++)
        {
                ress = mpu_dmp_init();
                if (ress == 0)
                {
                        UMPU_Euler_Angle.Init_Flag = true;
                        break;
                }
                MY_DEBUG("MPUError:%d.", ress);
        }

        printf("/*********ready***************/\n");

        Wifi_Init(); // wifi初始化

        xTaskCreate(&SHT20_Test, "SHT20_Test", 1024, NULL, Priority_SHT20, &SHT20_Test_Handle);

        if (UMPU_Euler_Angle.Init_Flag == true)
        {
                // xTaskCreate(&MPU_Test, "MPU_Test", 4096, NULL, Priority_MUP, &MPU_Test_Handle);
        }
        xTaskCreate(&Power_Test, "Power_Test", 2048, NULL, Priority_Power, &Power_Test_Handle);

        /*防止LVGL任务超时阻塞,使用xTaskCreatePinnedToCore(复活)*/

        xTaskCreatePinnedToCore(&gui_task, "LVGL_gui", 4096 * 2, NULL, Priority_LVGL, NULL, 1);

        // xTaskCreate(&Watch_test, "Watch_test", 1024, NULL, 17, NULL);

        printf("/*******************************************/\n");

        while (1)
        {
                MY_DEBUG("最小空闲堆大小%fKB.", (float)esp_get_minimum_free_heap_size() / 1024);
                MY_DEBUG("内部RAM剩余%dKB  ...", heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024);
                MY_DEBUG("芯片可用内存:%fKB", ((float)esp_get_free_heap_size() / 1024));
                MY_DEBUG("芯片可用内部内存:%fKB", ((float)esp_get_free_internal_heap_size() / 1024));

                // vTaskList((char *)&InfoBuffer);
                // printf("任务名|      任务状态|优先级 | 剩余栈|任务序号|CORE\r\n");
                // printf("\r\n%s\r\n", InfoBuffer);

                if (U_wifi_data.State == Wifi_connected)
                {
                        MY_DEBUG("tody.date:%s.", WData_All.today.date);
                        MY_DEBUG("tody.text_day:%s.", WData_All.today.text_day);
                        MY_DEBUG("tody.text_night:%s.", WData_All.today.text_night);
                        MY_DEBUG("tody.high:%s.", WData_All.today.high);
                        MY_DEBUG("tody.low:%s.", WData_All.today.low);
                        MY_DEBUG("tody.rainfall:%s.", WData_All.today.rainfall);
                        MY_DEBUG("tody.humidity:%s.", WData_All.today.humidity);
                        MY_DEBUG("tody.wind_direction:%s.", WData_All.today.wind_direction);
                        MY_DEBUG("tody.wind_direction_degree:%s.", WData_All.today.wind_direction_degree);
                        MY_DEBUG("tody.wind_speed:%s.", WData_All.today.wind_speed);
                        MY_DEBUG("tody.wind_scale:%s.", WData_All.today.wind_scale);
                        MY_DEBUG("tody-Time:%s...", Time_Data.time);
                }

                if (SHT_Value.SHT_Flag == true)
                {
                        SHT_Value.SHT_Flag = false;
                        MY_DEBUG("SHT20_T:%.2fC.", SHT_Value.SHT_T);
                        MY_DEBUG("SHT20_H:%.2f%%.", SHT_Value.SHT_H);
                }
                if (MPU_Value.Data_Flag == true)
                {
                        MPU_Value.Data_Flag = false;
                        MY_DEBUG("MPU_T:->%.2fC.", MPU_Value.Temp);
                        MY_DEBUG("MPU_pitch-->%.2f.", MPU_Value.pitch);
                        MY_DEBUG("MPU_roll--->%.2f.", MPU_Value.roll);
                        MY_DEBUG("MPU_yaw---->%.2f.", MPU_Value.yaw);
                }
                vTaskDelay(1000 / portTICK_RATE_MS);
        }
}
