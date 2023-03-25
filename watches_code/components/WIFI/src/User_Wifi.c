#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "User_Wifi.h"
#include "web_serve.h"
#include "Http_Weather.h"
#include "Http_time.h"
#include "main.h"

/*定义wifi结构体函数*/
_U_WIFI_DATA U_wifi_data;

#define EXAMPLE_ESP_WIFI_SSID "baymax"    // 默认 wifi名称
#define EXAMPLE_ESP_WIFI_PASS "123456789" // 默认 wifi密码
#define EXAMPLE_ESP_MAXIMUM_RETRY 3       // 尝试连接次数
static void wifi_connected_http(void);
/**
 * @brief wifi链接任务
 *
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (U_wifi_data.State == Wifi_request_break)
    {
      U_wifi_data.State = Wifi_connection_break;
      wifi_testAll_stop();
    }
    else
    {
      MY_DEBUG("wifi error");
      U_wifi_data.State = Wifi_connection_fail;
    }
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    MY_DEBUG("got ip:%d.%d.%d,%d;", IP2STR(&event->ip_info.ip));
    sprintf(U_wifi_data.ip, "%d.%d.%d.%d", IP2STR(&event->ip_info.ip));
    if (U_wifi_data.wifi_test == false)
    {
      wifi_connected_http();
      U_wifi_data.wifi_test = true;
    }
    else if (U_wifi_data.wifi_test == true)
    {
      MY_DEBUG("wifi相关任务恢复");
      wifi_testall_start();
    }
    U_wifi_data.State = Wifi_connected;
  }

  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
  {
    MY_DEBUG("扫描结束");
    wifi_get_scanList();
  }
}
/**
 * @brief wifi初始化
 */
esp_err_t Wifi_Init(void)
{
  U_wifi_data.State = Wifi_Not_connected;
  U_wifi_data.wifi_test = false;
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;

  memcpy(U_wifi_data.SSID, EXAMPLE_ESP_WIFI_SSID, strlen(EXAMPLE_ESP_WIFI_SSID));
  memcpy(U_wifi_data.PASS, EXAMPLE_ESP_WIFI_PASS, strlen(EXAMPLE_ESP_WIFI_PASS));

  ESP_ERROR_CHECK(
      esp_event_handler_instance_register(
          WIFI_EVENT,
          ESP_EVENT_ANY_ID,
          &event_handler,
          NULL,
          &instance_any_id));
  ESP_ERROR_CHECK(
      esp_event_handler_instance_register(
          IP_EVENT,
          IP_EVENT_STA_GOT_IP,
          &event_handler,
          NULL,
          &instance_got_ip));

  ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, U_wifi_data.MAC));
  MY_DEBUG("WIFI MAC:%02x:%02x:%02x:%02x:%02x:%02x.", U_wifi_data.MAC[0], U_wifi_data.MAC[1],
           U_wifi_data.MAC[2], U_wifi_data.MAC[3], U_wifi_data.MAC[4], U_wifi_data.MAC[5]);
  wifi_connect_ap(0, U_wifi_data.SSID, U_wifi_data.PASS, NULL);
  return ESP_OK;
}
/**
 * @brief 获取可用wifi
 *
 */
void wifi_get_scanList(void)
{
  uint16_t number = WIFI_AP_NUM_MAX; // 默认扫描列表大小
  U_wifi_data.Scanlist_num = 0;
  MY_DEBUG("获取wifi列表..");
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, U_wifi_data.wifi_ap_info)); // 获取搜索的具体AP信息
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&U_wifi_data.Scanlist_num));             // 接入点的数量
  MY_DEBUG("可用wifi数量:%d.", U_wifi_data.Scanlist_num);
  for (int i = 0; i < U_wifi_data.Scanlist_num; i++)
  {
    MY_DEBUG("wifi名:%s wiif信号强度:%d wifi通道 %d.",
             U_wifi_data.wifi_ap_info[i].ssid,
             U_wifi_data.wifi_ap_info[i].rssi,
             U_wifi_data.wifi_ap_info[i].primary);
  }
  esp_wifi_scan_stop();
  U_wifi_data.wifi_scan_flag = false;
}
/**
 * @brief 开始扫描可用wifi
 *
 */
void wifi_scanList_start(void)
{
  MY_DEBUG("开始扫描wifi列表\r\n");
  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false)); // 开始扫描
}
/**
 * @brief wifi连接
 *
 * @param mode
 * @param name
 * @param password
 * @param cb
 */
void wifi_connect_ap(uint8_t mode, char *name, char *password, void (*cb)(char *text, void *dat))
{
  static uint8_t flag = 0;
  U_wifi_data.cb = cb;
  if (name == NULL || password == NULL || strlen(name) == 0 || strlen(password) == 0)
  {
    MY_DEBUG("参数错误 无法连接wifi");
    return;
  }
  memset(&U_wifi_data.wifi_config, 0x00, sizeof(wifi_config_t));
  memcpy(U_wifi_data.wifi_config.sta.ssid, name, strlen(name));
  memcpy(U_wifi_data.wifi_config.sta.password, password, strlen(password));
  if (U_wifi_data.State == Wifi_connected)
  {
    MY_DEBUG("断开连接");
    U_wifi_data.State = Wifi_request_break;
    ESP_ERROR_CHECK(esp_wifi_disconnect()); // 断开连接
    while (U_wifi_data.State != Wifi_connection_break)
    {
      vTaskDelay(100);
    }
    MY_DEBUG("已断开");
  }
  MY_DEBUG("SSID:%s.", U_wifi_data.wifi_config.sta.ssid);
  MY_DEBUG("PASS:%s.", U_wifi_data.wifi_config.sta.password);
  if (flag == 0)
  {
    flag = 1;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &U_wifi_data.wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
  }
  else
  {
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &U_wifi_data.wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
  }
  U_wifi_data.State = Wifi_conecting;
}
/**
 * @brief wifi连接成功创建相关任务
 *
 */
static void wifi_connected_http(void)
{
  Web_Serve(); // web 主机

  /*webserver 任务处理字符串 被置于cpu1中 无法挂起*/
  xTaskCreatePinnedToCore(&Web_Test, "Web_Test", 2048, NULL, Priority_Web, &Web_Test_Handle, 0);
  xTaskCreate(&Weather_test_task, "Weather_test", 4096, NULL, Priority_Weather, &Weather_test_Handle);
  xTaskCreate(&Time_test_task, "Time_test", 3072, NULL, Priority_Time, &Time_test_Handle);
}
/**
 * @brief 断开wifi连接
 *
 */
void Wifi_connect_break(void)
{
  if (U_wifi_data.State == Wifi_connected)
  {
    MY_DEBUG("断开连接");
    U_wifi_data.State = Wifi_request_break;
    ESP_ERROR_CHECK(esp_wifi_disconnect()); // 断开连接
    while (U_wifi_data.State != Wifi_connection_break)
    {
      vTaskDelay(100);
    }
    MY_DEBUG("已断开");
  }
}
/**
 * @brief wifi相关任务挂起
 *
 */
void wifi_testAll_stop(void)
{
  vTaskSuspend(Weather_test_Handle);
  vTaskSuspend(Time_test_Handle);
  vTaskSuspend(Web_Test_Handle);
  // stop_web_server();
}
/**
 * @brief wifi相关任务恢复
 *
 */
void wifi_testall_start(void)
{
  vTaskResume(Weather_test_Handle);
  vTaskResume(Time_test_Handle);
  vTaskResume(Web_Test_Handle);
  // Web_Serve();
}
