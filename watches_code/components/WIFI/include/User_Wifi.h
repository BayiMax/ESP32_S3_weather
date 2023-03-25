/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-02 22:34:06
 * @FilePath: \esps3_V1\components\WIFI\include\User_Wifi.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef USER_WIFI_H
#define USER_WIFI_H

#include "main.h"

#define WIFI_AP_NUM_MAX 20

typedef struct
{
  char SSID[32];
  char PASS[64];
  wifi_config_t wifi_config;
  char State;
  bool wifi_test;
  void (*cb)(char *text, void *dat);
  uint8_t MAC[6];
  uint16_t Scanlist_num;
  wifi_ap_record_t wifi_ap_info[WIFI_AP_NUM_MAX];
  bool wifi_scan_flag;
  char ip[30];               // IP地址
  unsigned short netmask[4]; // 子网掩码
  unsigned short gateway[4]; // 网关IP地址
} _U_WIFI_DATA;

enum wifi_state
{
  Wifi_connected = 0,    // 已连接
  Wifi_Not_connected,    // 未连接
  Wifi_conecting,        // 连接中
  Wifi_connection_fail,  // 连接失败
  Wifi_connection_break, // 连接断开
  Wifi_request_break,    // 请求断开
};

extern _U_WIFI_DATA U_wifi_data;

/*函数声明*/
esp_err_t Wifi_Init(void);
void wifi_scanList_start(void);
void wifi_get_scanList(void);
void wifi_connect_ap(uint8_t mode, char *name, char *password, void (*cb)(char *text, void *dat));
void wifi_testall_start(void);
void wifi_testAll_stop(void);
void Wifi_connect_break(void);
#endif /* USER_WIFI_H */
