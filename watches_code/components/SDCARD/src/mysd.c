/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-27 00:17:41
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-30 23:53:04
 * @FilePath: \esps3_V1\components\SDCARD\src\mysd.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-27 00:17:41
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-27 14:53:15
 * @FilePath: \esps3_V1\components\SDCARD\src\mysd.c
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
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

#include "main.h"

#include "mysd.h"

#define MOUNT_POINT "/sdcard"

_SD_TYP SDTyp;

bool SD_card_init(void)
{
  esp_err_t ret;
  esp_vfs_fat_sdmmc_mount_config_t mysd_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};
  sdmmc_card_t *card;
  const char mount_point[] = MOUNT_POINT;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 4;

  slot_config.clk = 16;
  slot_config.cmd = 15;
  slot_config.d0 = 17;
  slot_config.d1 = 18;
  slot_config.d2 = 13;
  slot_config.d3 = 14;

  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mysd_config, &card);
  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      MY_DEBUG("SD fat error!");
    }
    else
    {
      MY_DEBUG("SD init error (%s)", esp_err_to_name(ret));
    }
    SDTyp.SDinit_Flag = false;
    return false;
  }
  MY_DEBUG("SD FAT is Ready!");

  MY_DEBUG("SD out %s.", card->cid.name);

  sdmmc_card_print_info(stdout, card);

  SDTyp.SDinit_Flag = true;

  return true;

  // sdmmc_card_print_info(mount_point, card);
}
