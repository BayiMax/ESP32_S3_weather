#ifndef HTTP_WEATHER_H
#define HTTP_WEATHER_H

#include "cJSON.h"

typedef struct
{
  char id[15];                    //地区ID
  char name[15];                  //地区名
  char country[15];               //国家
  char path[15];                  //详细地区信息
  char timezone[15];              //时区
  char timezone_offset[15];       //时区差
  char date[15];                  //日期
  char text_day[15];              //白天天气现象文字
  char code_day[15];              //白天天气现象代码
  char text_night[15];            //夜晚天气现象文字
  char code_night[15];            //夜晚天气现象代码
  char high[15];                  //最高温度，单位为c摄氏度或f华氏度
  char low[15];                   //最低温度，单位为c摄氏度或f华氏度
  char rainfall[15];              //降雨量
  char precip[15];                //
  char feels_like[15];            //体感温度，单位为c摄氏度或f华氏度
  char pressure[15];              //气压，单位为mb百帕或in英寸
  char humidity[15];              //相对湿度，0~100，单位为百分比
  char visibility[15];            //能见度，单位为km公里或mi英里
  char wind_direction[15];        //风向文字
  char wind_direction_degree[15]; //风向角度，范围0~360，0为正北，90为正东，180为正南，270为正西
  char wind_speed[15];            //风速，单位为km/h公里每小时或mph英里每小时
  char wind_scale[15];            //风力等级，请参考：http://baike.baidu.com/view/465076.htm
  char clouds[15];                //云量，单位%，范围0~100，天空被云覆盖的百分比 #目前不支持中国城市#
  char dew_point[15];             //露点温度，请参考：http://baike.baidu.com/view/118348.htm #目前不支持中国城市#
  char last_update[15];           //数据更新时间（该城市的本地时间）
} _Weather_data;

typedef struct
{
  _Weather_data today;
  _Weather_data tomorrow;
  _Weather_data after_tomorrow;
  volatile bool Weather_Flag;
} _Weather_day;

extern _Weather_day WData_All;

void Get_Weather_Data(_Weather_day SWdata);

void Weather_test_task(void *pvParameters);
char *Get_cJSON_Vlaue(const cJSON *const object, const char *const string);
// void Weather_Get_task_init(void);

#endif /* HTTP_WEATHER_H */
