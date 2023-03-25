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
#include "Http_Weather.h"
#include "Watch.h"

#define MAX_HTTP_OUTPUT_BUFFER 1024
#define HOST "api.seniverse.com"
#define UserKey "SBoL5FbXW4IufElLM"
#define Location "chongqing"
#define Language "en"
#define Strat "0"
#define Days "3"

_Weather_day WData_All;

char *Get_cJSON_Vlaue(const cJSON *const object, const char *const string)
{
  cJSON *JsonDate;
  JsonDate = cJSON_GetObjectItem(object, string);
  if (cJSON_IsString(JsonDate))
  {
    return JsonDate->valuestring;
  }
  else
    return 0;
}
/**
 * @author BBX
 * @date 2022年9月27日
 * @brief 解析数据包中json数据
 *
 * @param text
 * @param WX_typ
 */
static void Analysis_WeatherJSON(char *text, _Weather_day *WX_typ)
{
  cJSON *root, *arrayItem, *subArray;
  cJSON *arr_item, *sub_array_item;
  int num;
  // cJSON *JsonDate;
  root = cJSON_Parse(text); // 返回可以查询的json数据
  if (root != NULL)
  {
    arrayItem = cJSON_GetObjectItem(root, "results"); // 从数据中查询字符
    int arr_size = cJSON_GetArraySize(arrayItem);
    // printf("......root_arr_size : %d \n", arr_size);
    arr_item = arrayItem->child;
    for (int i = 0; i < arr_size; i++)
    {
      subArray = cJSON_GetObjectItem(arr_item, "daily");
      int sub_array_size = cJSON_GetArraySize(subArray);
      sub_array_item = subArray->child;
      // printf(".....sub_arr_size: %d \n", sub_array_size);
      num = sub_array_size / 3;
      /*获取今天信息*/
      for (int j = 0; j < num; j++)
      {
        if (sub_array_item->type == cJSON_Object)
        {
          sprintf(WX_typ->today.date, "%s", Get_cJSON_Vlaue(sub_array_item, "date"));
          printf("____today date:%s is OK\n", WX_typ->today.date);
          sprintf(WX_typ->today.text_day, "%s", Get_cJSON_Vlaue(sub_array_item, "text_day"));
          sprintf(WX_typ->today.code_day, "%s", Get_cJSON_Vlaue(sub_array_item, "code_day"));

          sprintf(WX_typ->today.text_night, "%s", Get_cJSON_Vlaue(sub_array_item, "text_night"));
          sprintf(WX_typ->today.code_night, "%s", Get_cJSON_Vlaue(sub_array_item, "code_night"));

          sprintf(WX_typ->today.high, "%s", Get_cJSON_Vlaue(sub_array_item, "high"));
          sprintf(WX_typ->today.low, "%s", Get_cJSON_Vlaue(sub_array_item, "low"));

          sprintf(WX_typ->today.rainfall, "%s", Get_cJSON_Vlaue(sub_array_item, "rainfall"));
          sprintf(WX_typ->today.precip, "%s", Get_cJSON_Vlaue(sub_array_item, "precip"));

          sprintf(WX_typ->today.wind_direction, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction"));
          sprintf(WX_typ->today.wind_direction_degree, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction_degree"));
          sprintf(WX_typ->today.wind_speed, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_speed"));
          sprintf(WX_typ->today.wind_scale, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_scale"));

          sprintf(WX_typ->today.humidity, "%s", Get_cJSON_Vlaue(sub_array_item, "humidity"));
        }
        sub_array_item = sub_array_item->next;
      }
      /*获取明天信息*/
      for (int j = num; j < (2 * num); j++)
      {
        if (sub_array_item->type == cJSON_Object)
        {
          sprintf(WX_typ->tomorrow.date, "%s", Get_cJSON_Vlaue(sub_array_item, "date"));
          printf("____tomorrow date:%s is OK\n", WX_typ->tomorrow.date);
          sprintf(WX_typ->tomorrow.text_day, "%s", Get_cJSON_Vlaue(sub_array_item, "text_day"));
          sprintf(WX_typ->tomorrow.code_day, "%s", Get_cJSON_Vlaue(sub_array_item, "code_day"));

          sprintf(WX_typ->tomorrow.text_night, "%s", Get_cJSON_Vlaue(sub_array_item, "text_night"));
          sprintf(WX_typ->tomorrow.code_night, "%s", Get_cJSON_Vlaue(sub_array_item, "code_night"));

          sprintf(WX_typ->tomorrow.high, "%s", Get_cJSON_Vlaue(sub_array_item, "high"));
          sprintf(WX_typ->tomorrow.low, "%s", Get_cJSON_Vlaue(sub_array_item, "low"));

          sprintf(WX_typ->tomorrow.rainfall, "%s", Get_cJSON_Vlaue(sub_array_item, "rainfall"));
          sprintf(WX_typ->tomorrow.precip, "%s", Get_cJSON_Vlaue(sub_array_item, "precip"));

          sprintf(WX_typ->tomorrow.wind_direction, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction"));
          sprintf(WX_typ->tomorrow.wind_direction_degree, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction_degree"));
          sprintf(WX_typ->tomorrow.wind_speed, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_speed"));
          sprintf(WX_typ->tomorrow.wind_scale, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_scale"));

          sprintf(WX_typ->tomorrow.humidity, "%s", Get_cJSON_Vlaue(sub_array_item, "humidity"));
        }
        sub_array_item = sub_array_item->next;
      }
      /*获取后天信息*/
      for (int j = (2 * num); j < (3 * num); j++)
      {
        if (sub_array_item->type == cJSON_Object)
        {
          // WX_typ->after_after_tomorrow.date = Get_cJSON_Vlaue(sub_array_item, "date");
          sprintf(WX_typ->after_tomorrow.date, "%s", Get_cJSON_Vlaue(sub_array_item, "date"));
          printf("____after_tomorrow date:%s is OK\n", WX_typ->after_tomorrow.date);
          sprintf(WX_typ->after_tomorrow.text_day, "%s", Get_cJSON_Vlaue(sub_array_item, "text_day"));
          sprintf(WX_typ->after_tomorrow.code_day, "%s", Get_cJSON_Vlaue(sub_array_item, "code_day"));

          sprintf(WX_typ->after_tomorrow.text_night, "%s", Get_cJSON_Vlaue(sub_array_item, "text_night"));
          sprintf(WX_typ->after_tomorrow.code_night, "%s", Get_cJSON_Vlaue(sub_array_item, "code_night"));

          sprintf(WX_typ->after_tomorrow.high, "%s", Get_cJSON_Vlaue(sub_array_item, "high"));
          sprintf(WX_typ->after_tomorrow.low, "%s", Get_cJSON_Vlaue(sub_array_item, "low"));

          sprintf(WX_typ->after_tomorrow.rainfall, "%s", Get_cJSON_Vlaue(sub_array_item, "rainfall"));
          sprintf(WX_typ->after_tomorrow.precip, "%s", Get_cJSON_Vlaue(sub_array_item, "precip"));

          sprintf(WX_typ->after_tomorrow.wind_direction, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction"));
          sprintf(WX_typ->after_tomorrow.wind_direction_degree, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_direction_degree"));
          sprintf(WX_typ->after_tomorrow.wind_speed, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_speed"));
          sprintf(WX_typ->after_tomorrow.wind_scale, "%s", Get_cJSON_Vlaue(sub_array_item, "wind_scale"));

          sprintf(WX_typ->after_tomorrow.humidity, "%s", Get_cJSON_Vlaue(sub_array_item, "humidity"));
        }
        sub_array_item = sub_array_item->next;
      }
      arr_item = arr_item->next;
    }
    WX_typ->Weather_Flag = true;
  }
  else
  {
    MY_DEBUG("Page not returned !!!!\n");
  }
  cJSON_Delete(root);
}

void Weather_test_task(void *pvParameters)
{
  char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
  int content_length = 0;
  int data_read;
  esp_err_t err;
  static const char *Weather_URL = "http://" HOST "/v3/weather/daily.json?"
                                   "key=" UserKey "&location=" Location "&language=" Language "&unit=c&start=" Strat "&days=" Days;
  esp_http_client_config_t http_config = {
      .url = Weather_URL,
      .method = HTTP_METHOD_GET,
  };

  esp_http_client_handle_t weather_client =
      esp_http_client_init(&http_config);
  while (1)
  {
    err = esp_http_client_open(weather_client, 0);
    if (err != ESP_OK)
    {
      // 连接失败
      MY_DEBUG("error");
    }
    else
    {
      content_length = esp_http_client_fetch_headers(weather_client);
      if (content_length < 0)
      {
        MY_DEBUG("error");
      }
      else
      {
        data_read = esp_http_client_read_response(
            weather_client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
        if (data_read >= 0)
        {
          Analysis_WeatherJSON(output_buffer, &WData_All);
        }
        else
        {
          MY_DEBUG("error");
        }
      }
    }
    MY_DEBUG("Weather test runing");
    Refresh_Weather_test();
    Refresh_timeDataOnce_test();
    esp_http_client_close(weather_client);
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}
