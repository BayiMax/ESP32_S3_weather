/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-08-30 20:19:40
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-01 23:58:51
 * @FilePath: \esps3_V1\components\HTTP\src\Http_time.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "main.h"
#include "Http_time.h"
#include "Http_Weather.h"
#include "watch.h"
#define MAX_TIME_OUTPUT_BUFFER 512
_TIME_Data Time_Data;
/**
 * @brief
 *
 * @param text
 * @param Read_Time
 */
static void Analsis_timeJSON(char *text, _TIME_Data *Read_Time)
{
  char Data_buf1[20];
  char Data_buf2[20];
  cJSON *root, *arrayItem;
  root = cJSON_Parse(text); // 返回可以查询的json数据
  if (root != NULL)
  {
    arrayItem = cJSON_GetObjectItem(root, "sysTime2");

    sprintf(Data_buf1, "%s", cJSON_GetStringValue(arrayItem));
    memcpy(Data_buf2, Data_buf1 + 11, sizeof(Data_buf1) - 11);
    sprintf(Read_Time->time, "%s", Data_buf2);
    MY_DEBUG("...time is %s\n", Read_Time->time);
    Read_Time->Time_Flag = true;
  }
  else
  {
    printf("----> %s,%d Page not returned !!!!\n ", __FILE__, __LINE__);
  }
  cJSON_Delete(root);
}
void Time_test_task(void *pvParameters)
{
  char Time_buffer[MAX_TIME_OUTPUT_BUFFER] = {0};
  int content_length = 0;
  int data_read;
  esp_err_t err;
  static const char *time_URL = "http://quan.suning.com/getSysTime.do";
  esp_http_client_config_t time_config = {
      .url = time_URL,
      .method = HTTP_METHOD_GET,
  };
  esp_http_client_handle_t Time_client =
      esp_http_client_init(&time_config);
  while (1)
  {
    err = esp_http_client_open(Time_client, 0);
    if (err != ESP_OK)
    {
      // 连接失败
      MY_DEBUG("error");
    }
    else
    {
      content_length = esp_http_client_fetch_headers(Time_client);
      if (content_length < 0)
      {
        MY_DEBUG("error");
      }
      else
      {
        data_read = esp_http_client_read_response(
            Time_client, Time_buffer, MAX_TIME_OUTPUT_BUFFER);
        if (data_read >= 0)
        {
          // printf("......HTTP GET Status =%,content_length=%d\n",
          //        esp_http_client_get_status_code(Time_client),
          //        esp_http_client_get_content_length(Time_client));
          Analsis_timeJSON(Time_buffer, &Time_Data);
        }
        else
        {
          printf("......Failed to read response\n");
        }
      }
    }
    MY_DEBUG("Time test runing");
    Time_TO_HttpTime();
    esp_http_client_close(Time_client);
    vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);
  }
}
