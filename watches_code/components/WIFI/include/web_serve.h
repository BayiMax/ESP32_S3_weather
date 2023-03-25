/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-20 19:21:37
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-11-01 20:33:23
 * @FilePath: \esps3_V1\components\WIFI\include\web_serve.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef WEB_SERVE_H
#define WEB_SERVE_H

#define HTTP_SHOW_LEN 1000

void Web_Serve(void);
void stop_web_server(void);
void Web_Test(void *pvParameters);
#endif /* WEB_SERVE_H */
